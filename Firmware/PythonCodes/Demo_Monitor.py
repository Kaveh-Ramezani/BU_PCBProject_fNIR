import sys
import os
import time
import struct
import threading
import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QComboBox, QPushButton, QMessageBox,
                             QTabWidget, QFormLayout, QSpinBox)
from PyQt5.QtCore import QThread, pyqtSignal
import pyqtgraph as pg

# --- 1. Background Worker Thread for Serial Data ---
class SerialWorker(QThread):
    # UPDATED: Added a 4th float for the 'DARK' data
    data_ready = pyqtSignal(float, float, float, float)
    error_occurred = pyqtSignal(str)

    def __init__(self, port, baudrate=115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.is_running = True
        self.serial_conn = None
        
        self.config_lock = threading.Lock()
        self.current_config = (0, 0) 

    def update_configuration(self, l1_int, l2_int):
        with self.config_lock:
            self.current_config = (l1_int, l2_int)

    def run(self):
        # Define a quick function to safely write the data
        def send_delayed_response():
            if self.serial_conn and self.serial_conn.is_open:
                self.serial_conn.write(response_packet)
        try:
            self.serial_conn = serial.Serial(self.port, self.baudrate, timeout=0.1)
            buffer = bytearray()
            start_time = time.perf_counter()

            config_request_payload = bytearray([0x4E, 0x65, 0x77, 0x43])

            while self.is_running:
                if self.serial_conn.in_waiting > 0:
                    buffer += self.serial_conn.read(self.serial_conn.in_waiting)
                    
                    # UPDATED: We now need at least 10 bytes for the ADC data packet
                    while len(buffer) >= 10:
                        # --- Packet Type 1: ADC Data (0x55 0x55 ...) ---
                        if buffer[0] == 0x55 and buffer[1] == 0x55:
                            # UPDATED: Footer is now at index 8 and 9
                            if buffer[8] == 0x0D and buffer[9] == 0x0A:
                                raw_data1 = (buffer[3] << 8) | buffer[2]
                                raw_data2 = (buffer[5] << 8) | buffer[4]
                                raw_dark  = (buffer[7] << 8) | buffer[6] # NEW: Dark data extraction
                                
                                data1 = (raw_data1 / 4095.0) * 3.3
                                data2 = (raw_data2 / 4095.0) * 3.3
                                dark  = (raw_dark / 4095.0) * 3.3 # Assuming Dark is also a 12-bit ADC value
                                
                                current_time = time.perf_counter() - start_time
                                
                                # Send 4 values to GUI
                                self.data_ready.emit(current_time, data1, data2, dark)
                                
                                response_packet = bytearray([0xAA])
                                delay_seconds = 0.0 # 50 milliseconds (Change this as needed)
                                # Start the timer without blocking the main read loop
                                threading.Timer(delay_seconds, send_delayed_response).start()
                                # UPDATED: Remove 10 bytes from buffer
                                buffer = buffer[10:]
                                continue
                            
                        # --- Packet Type 2: Configuration Request (0xAA 0xAA ...) ---
                        elif buffer[0] == 0xAA and buffer[1] == 0xAA:
                            if buffer[2:6] == config_request_payload and buffer[6] == 0x0D and buffer[7] == 0x0A:
                                with self.config_lock:
                                    cfg = self.current_config
                                
                                payload_bytes = struct.pack('<HH', cfg[0], cfg[1])
                                response_packet = bytearray([0xAA, 0xAA]) + payload_bytes + bytearray([0x0D, 0x0A])
                                
                                # --- NEW DELAY LOGIC ---
                                delay_seconds = 0.0 # 50 milliseconds (Change this as needed)
                                # Start the timer without blocking the main read loop
                                threading.Timer(delay_seconds, send_delayed_response).start()
                                print(f"Delayed response sent: {response_packet.hex().upper()}")
                                # -----------------------
                                
                                buffer = buffer[8:]
                                continue

                        # If no match, slide window by 1
                        buffer = buffer[1:]

        except Exception as e:
            self.error_occurred.emit(str(e))
        finally:
            if self.serial_conn and self.serial_conn.is_open:
                self.serial_conn.close()

    def stop(self):
        self.is_running = False
        self.wait()

# --- 2. Main GUI Application ---
class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("LED Controller & Data Monitor")
        self.resize(1000, 600)

        self.serial_worker = None
        
        # Data lists
        self.time_data = []
        self.data1_list = []
        self.data2_list = []
        self.dark_list = [] # NEW: Storage for Dark data

        self.init_ui()

    def init_ui(self):
        self.tabs = QTabWidget()
        self.setCentralWidget(self.tabs)

        self.tab_settings = QWidget()
        self.tab_monitor = QWidget()

        self.tabs.addTab(self.tab_settings, "Settings")
        self.tabs.addTab(self.tab_monitor, "Monitor")

        self.setup_settings_tab()
        self.setup_monitor_tab()

    def setup_settings_tab(self):
        layout = QFormLayout()

        self.spin_led1_intensity = QSpinBox()
        self.spin_led1_intensity.setRange(0, 100)
        self.spin_led1_intensity.setSuffix(" %")
        layout.addRow("LED 1 Intensity:", self.spin_led1_intensity)

        self.spin_led2_intensity = QSpinBox()
        self.spin_led2_intensity.setRange(0, 100)
        self.spin_led2_intensity.setSuffix(" %")
        layout.addRow("LED 2 Intensity:", self.spin_led2_intensity)

        self.btn_apply_settings = QPushButton("Apply Settings")
        self.btn_apply_settings.clicked.connect(self.apply_settings)
        layout.addRow("", self.btn_apply_settings)

        self.tab_settings.setLayout(layout)
    
    def setup_monitor_tab(self):
        main_layout = QVBoxLayout()

        # Real-time Graph
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground('w')
        self.plot_widget.setTitle("Real-Time Data (5 Minute Span)", color="k")
        self.plot_widget.setLabel('left', 'Voltage (V)', color='k')
        self.plot_widget.setLabel('bottom', 'Time (s)', color='k')
        self.plot_widget.addLegend()
        self.plot_widget.showGrid(x=True, y=True)

        self.curve1 = self.plot_widget.plot(pen=pg.mkPen('b', width=2), name="DATA 1")
        self.curve2 = self.plot_widget.plot(pen=pg.mkPen('r', width=2), name="DATA 2")
        self.curve3 = self.plot_widget.plot(pen=pg.mkPen('g', width=2), name="DARK") # NEW: Green curve for Dark
        
        main_layout.addWidget(self.plot_widget)

        # Bottom Controls
        bottom_layout = QHBoxLayout()

        self.combo_ports = QComboBox()
        self.refresh_ports()
        bottom_layout.addWidget(self.combo_ports)

        # Added a quick refresh button for convenience!
        self.btn_refresh = QPushButton("Refresh")
        self.btn_refresh.clicked.connect(self.refresh_ports)
        bottom_layout.addWidget(self.btn_refresh)

        self.btn_connect = QPushButton("Connect")
        self.btn_connect.clicked.connect(self.connect_serial)
        bottom_layout.addWidget(self.btn_connect)

        self.btn_disconnect = QPushButton("Disconnect")
        self.btn_disconnect.clicked.connect(self.disconnect_serial)
        self.btn_disconnect.setEnabled(False)
        bottom_layout.addWidget(self.btn_disconnect)

        bottom_layout.addStretch()

        self.btn_stop = QPushButton("Stop & Save")
        self.btn_stop.clicked.connect(self.stop_and_save)
        self.btn_stop.setStyleSheet("background-color: #ff4c4c; color: white; font-weight: bold;")
        bottom_layout.addWidget(self.btn_stop)

        main_layout.addLayout(bottom_layout)
        self.tab_monitor.setLayout(main_layout)

    def apply_settings(self):
        l1_int = self.spin_led1_intensity.value()
        l2_int = self.spin_led2_intensity.value()
        
        if self.serial_worker and self.serial_worker.is_running:
            self.serial_worker.update_configuration(l1_int, l2_int)
            print(f"Settings pushed: LED1: {l1_int}%, LED2: {l2_int}%")
        else:
            print(f"Settings saved locally: LED1: {l1_int}%, LED2: {l2_int}% (will apply when connected).")
    
    def handle_error(self, err_msg):
        QMessageBox.critical(self, "Serial Error", f"An error occurred:\n{err_msg}")
        self.disconnect_serial()

    def refresh_ports(self):
        self.combo_ports.clear()
        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.combo_ports.addItem(port.device)

    def connect_serial(self):
        port = self.combo_ports.currentText()
        if not port:
            QMessageBox.warning(self, "Error", "No COM port selected.")
            return

        # Clear previous data
        self.time_data.clear()
        self.data1_list.clear()
        self.data2_list.clear()
        self.dark_list.clear() # Clear Dark data

        self.serial_worker = SerialWorker(port)
        self.serial_worker.data_ready.connect(self.update_data)
        self.serial_worker.error_occurred.connect(self.handle_error)
        self.serial_worker.start()

        self.btn_connect.setEnabled(False)
        self.btn_disconnect.setEnabled(True)
        self.combo_ports.setEnabled(False)
        self.btn_refresh.setEnabled(False)

    def disconnect_serial(self):
        if self.serial_worker and self.serial_worker.is_running:
            self.serial_worker.stop()
        
        self.btn_connect.setEnabled(True)
        self.btn_disconnect.setEnabled(False)
        self.combo_ports.setEnabled(True)
        self.btn_refresh.setEnabled(True)

    # UPDATED: Accept dark as parameter
    def update_data(self, t, d1, d2, dark):
        self.time_data.append(t)
        self.data1_list.append(d1)
        self.data2_list.append(d2)
        self.dark_list.append(dark) # NEW: Append dark data

        self.curve1.setData(self.time_data, self.data1_list)
        self.curve2.setData(self.time_data, self.data2_list)
        self.curve3.setData(self.time_data, self.dark_list) # NEW: Update dark curve

        if t > 300:
            self.plot_widget.setXRange(t - 300, t)
        else:
            self.plot_widget.setXRange(0, 300)

    def stop_and_save(self):
        self.disconnect_serial()
        
        if not self.time_data:
            QMessageBox.information(self, "Info", "No data to save.")
            return

        if not os.path.exists("Output"):
            os.makedirs("Output")

        filename = f"Output/acquired_data_{time.strftime('%Y%m%d_%H%M%S')}.txt"
        try:
            with open(filename, 'w') as f:
                # UPDATED: Added DARK to the headers
                f.write("Timestamp(s)\tDelta_t(s)\tDATA1\tDATA2\tDARK\n")
                
                for i in range(len(self.time_data)):
                    t = self.time_data[i]
                    d1 = self.data1_list[i]
                    d2 = self.data2_list[i]
                    dk = self.dark_list[i] # Get dark value
                    
                    delta_t = 0.0 if i == 0 else (self.time_data[i] - self.time_data[i-1])
                    
                    # UPDATED: Added dk to the output line
                    f.write(f"{t:.4f}\t{delta_t:.6f}\t{d1:.4f}\t{d2:.4f}\t{dk:.4f}\n")
            
            QMessageBox.information(self, "Success", f"Data saved successfully to {filename}")
        except Exception as e:
            QMessageBox.critical(self, "Save Error", f"Could not save file:\n{e}")

# --- 3. Application Execution ---
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())