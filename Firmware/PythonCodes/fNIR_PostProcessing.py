import math
from collections import deque
from statistics import median
import numpy as np


class RealTimeFNIRS:
    """
    Real-time 2-wavelength fNIRS processor for one detector.

    Per cycle input:
        timestamp, wl1_raw, wl2_raw, dark_raw

    Per cycle output:
        delta_hbo, delta_hbr

    Processing:
        1) dark subtraction
        2) median filtering
        3) optical density using running baseline
        4) MBLL conversion to HbO / HbR
    """

    def __init__(
        self,
        # Your wavelengths are same as paper
        lambda1_nm=740,
        lambda2_nm=860,

        # Same spacing as paper / your setup
        source_detector_distance_cm=3.0,

        # Paper's DPF values for forehead
        dpf1=4.38,   # 740 nm
        dpf2=3.94,   # 860 nm

        # Extinction coefficients
        # Replace these with your exact preferred values if needed.
        # Units just need to be consistent with each other and path length.
        eps_hbo_wl1=0.45,
        eps_hbr_wl1=1.10,
        eps_hbo_wl2=1.10,
        eps_hbr_wl2=0.45,

        # Real-time baseline settings
        baseline_alpha=0.01,      # slow baseline update
        baseline_init_samples=30, # wait this many good samples before output
        median_window=5,          # small real-time window
        min_intensity=1e-9        # avoid log/division issues
    ):
        self.lambda1_nm = lambda1_nm
        self.lambda2_nm = lambda2_nm
        self.L = source_detector_distance_cm
        self.dpf1 = dpf1
        self.dpf2 = dpf2
        self.baseline_alpha = baseline_alpha
        self.baseline_init_samples = baseline_init_samples
        self.median_window = median_window
        self.min_intensity = min_intensity

        # Buffers for small real-time median filtering
        self.buf1 = deque(maxlen=median_window)
        self.buf2 = deque(maxlen=median_window)

        # Running baselines
        self.baseline1 = None
        self.baseline2 = None
        self.good_sample_count = 0

        # Precompute MBLL inverse matrix
        # OD = A @ [dHbO, dHbR]^T  -> [dHbO, dHbR]^T = inv(A) @ OD
        A = np.array([
            [eps_hbo_wl1 * dpf1 * self.L, eps_hbr_wl1 * dpf1 * self.L],
            [eps_hbo_wl2 * dpf2 * self.L, eps_hbr_wl2 * dpf2 * self.L]
        ], dtype=float)

        detA = np.linalg.det(A)
        if abs(detA) < 1e-12:
            raise ValueError("MBLL matrix is singular or nearly singular. Check coefficients/DPF/L.")

        self.A_inv = np.linalg.inv(A)

    def _update_baseline(self, x, baseline):
        if baseline is None:
            return x
        return (1.0 - self.baseline_alpha) * baseline + self.baseline_alpha * x

    def _safe_log_ratio(self, signal, baseline):
        signal = max(signal, self.min_intensity)
        baseline = max(baseline, self.min_intensity)
        return -math.log(signal / baseline)

    def update(self, timestamp, wl1_raw, wl2_raw, dark_raw):
        """
        Process one acquisition cycle.

        Returns dict with:
            {
              "timestamp": ...,
              "wl1_corr": ...,
              "wl2_corr": ...,
              "od1": ... or None,
              "od2": ... or None,
              "delta_hbo": ... or None,
              "delta_hbr": ... or None,
              "ready": bool
            }
        """

        # 1) Dark subtraction
        wl1_corr = wl1_raw - dark_raw
        wl2_corr = wl2_raw - dark_raw

        # Reject invalid corrected intensities
        if wl1_corr <= self.min_intensity or wl2_corr <= self.min_intensity:
            return {
                "timestamp": timestamp,
                "wl1_corr": wl1_corr,
                "wl2_corr": wl2_corr,
                "od1": None,
                "od2": None,
                "delta_hbo": None,
                "delta_hbr": None,
                "ready": False
            }

        # 2) Small median filter
        self.buf1.append(wl1_corr)
        self.buf2.append(wl2_corr)

        filt1 = median(self.buf1)
        filt2 = median(self.buf2)

        # 3) Initialize/update baseline
        if self.baseline1 is None:
            self.baseline1 = filt1
            self.baseline2 = filt2
            self.good_sample_count = 1
            return {
                "timestamp": timestamp,
                "wl1_corr": filt1,
                "wl2_corr": filt2,
                "od1": None,
                "od2": None,
                "delta_hbo": None,
                "delta_hbr": None,
                "ready": False
            }

        # Warm-up period before reporting Hb
        if self.good_sample_count < self.baseline_init_samples:
            self.baseline1 = self._update_baseline(filt1, self.baseline1)
            self.baseline2 = self._update_baseline(filt2, self.baseline2)
            self.good_sample_count += 1

            return {
                "timestamp": timestamp,
                "wl1_corr": filt1,
                "wl2_corr": filt2,
                "od1": None,
                "od2": None,
                "delta_hbo": None,
                "delta_hbr": None,
                "ready": False
            }

        # 4) Optical density
        od1 = self._safe_log_ratio(filt1, self.baseline1)
        od2 = self._safe_log_ratio(filt2, self.baseline2)

        OD = np.array([od1, od2], dtype=float)

        # 5) MBLL conversion
        delta_hbo, delta_hbr = self.A_inv @ OD

        # 6) Slowly update baseline after computing output
        self.baseline1 = self._update_baseline(filt1, self.baseline1)
        self.baseline2 = self._update_baseline(filt2, self.baseline2)
        self.good_sample_count += 1

        return {
            "timestamp": timestamp,
            "wl1_corr": filt1,
            "wl2_corr": filt2,
            "od1": od1,
            "od2": od2,
            "delta_hbo": float(delta_hbo),
            "delta_hbr": float(delta_hbr),
            "ready": True
        }
		
		
		