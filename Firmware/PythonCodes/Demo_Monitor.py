import sys
import time
import serial
import serial.tools.list_ports
from PyQt5.QtWidgets import (QApplication, QMainWindow, QWidget, QVBoxLayout, 
                             QHBoxLayout, QComboBox, QPushButton, QMessageBox)
from PyQt5.QtCore import QThread, pyqtSignal
import pyqtgraph as pg

# --- 1. Background Worker Thread for Serial Data ---
class SerialWorker(QThread):
    # Signal to send data to the main GUI thread: (timestamp, data1, data2)
    data_ready = pyqtSignal(float, float, float)
    error_occurred = pyqtSignal(str)

    def __init__(self, port, baudrate=115200):
        super().__init__()
        self.port = port
        self.baudrate = baudrate
        self.is_running = True
        self.serial_conn = None

    def run(self):
        try:
            self.serial_conn = serial.Serial(self.port, self.baudrate, timeout=0.1)
            buffer = bytearray()
            start_time = time.perf_counter()

            while self.is_running:
                if self.serial_conn.in_waiting > 0:
                    # Read available bytes and add to our rolling buffer
                    buffer += self.serial_conn.read(self.serial_conn.in_waiting)
                    
                    # Search for the packet
                    while len(buffer) >= 8:
                        # Check for Header: 0x55 0x55
                        if buffer[0] == 0x55 and buffer[1] == 0x55:
                            # Check for Footer: 0x0D 0x0A
                            if buffer[6] == 0x0D and buffer[7] == 0x0A:
                                # Extract data using bitwise operations (Little Endian)
                                data1 = ((buffer[3] << 8) | buffer[2])
                                data1 = (data1/4095.0)*3.3
                                data2 = ((buffer[5] << 8) | buffer[4])
                                data2 = (data2/4095.0)*3.3
                                
                                current_time = time.perf_counter() - start_time
                                
                                # Send to GUI
                                self.data_ready.emit(current_time, data1, data2)
                                
                                # Remove the processed packet from the buffer
                                buffer = buffer[8:]
                            else:
                                # Footer mismatch, slide window by 1 to recover
                                buffer = buffer[1:]
                        else:
                            # Header mismatch, slide window by 1
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
        self.setWindowTitle("Serial Data Acquisition & Plotter")
        self.resize(1000, 600)

        self.serial_worker = None
        
        # Data storage for plotting and saving
        self.time_data = []
        self.data1_list = []
        self.data2_list = []

        self.init_ui()

    def init_ui(self):
        # Central Widget and Layouts
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        main_layout = QVBoxLayout(central_widget)

        # --- Middle: Real-time Graph ---
        self.plot_widget = pg.PlotWidget()
        self.plot_widget.setBackground('w')
        self.plot_widget.setTitle("Real-Time Data (5 Minute Span)", color="k")
        self.plot_widget.setLabel('left', 'Value', color='k')
        self.plot_widget.setLabel('bottom', 'Time (s)', color='k')
        self.plot_widget.addLegend()
        self.plot_widget.showGrid(x=True, y=True)

        self.curve1 = self.plot_widget.plot(pen=pg.mkPen('b', width=2), name="DATA 1")
        self.curve2 = self.plot_widget.plot(pen=pg.mkPen('r', width=2), name="DATA 2")
        
        main_layout.addWidget(self.plot_widget)

        # --- Bottom: Controls ---
        bottom_layout = QHBoxLayout()

        # Left Bottom Controls
        self.combo_ports = QComboBox()
        self.refresh_ports() # Populate available ports
        bottom_layout.addWidget(self.combo_ports)

        self.btn_connect = QPushButton("Connect")
        self.btn_connect.clicked.connect(self.connect_serial)
        bottom_layout.addWidget(self.btn_connect)

        self.btn_disconnect = QPushButton("Disconnect")
        self.btn_disconnect.clicked.connect(self.disconnect_serial)
        self.btn_disconnect.setEnabled(False)
        bottom_layout.addWidget(self.btn_disconnect)

        # Push the "Stop" button to the far right
        bottom_layout.addStretch()

        # Right Bottom Controls
        self.btn_stop = QPushButton("Stop & Save")
        self.btn_stop.clicked.connect(self.stop_and_save)
        # Apply some styling to make the Stop button prominent
        self.btn_stop.setStyleSheet("background-color: #ff4c4c; color: white; font-weight: bold;")
        bottom_layout.addWidget(self.btn_stop)

        main_layout.addLayout(bottom_layout)

    def refresh_ports(self):
        """Finds all available COM ports and adds them to the combo box."""
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

        # Start the background thread
        self.serial_worker = SerialWorker(port)
        self.serial_worker.data_ready.connect(self.update_data)
        self.serial_worker.error_occurred.connect(self.handle_error)
        self.serial_worker.start()

        # Update UI state
        self.btn_connect.setEnabled(False)
        self.btn_disconnect.setEnabled(True)
        self.combo_ports.setEnabled(False)

    def disconnect_serial(self):
        if self.serial_worker and self.serial_worker.is_running:
            self.serial_worker.stop()
        
        self.btn_connect.setEnabled(True)
        self.btn_disconnect.setEnabled(False)
        self.combo_ports.setEnabled(True)

    def update_data(self, t, d1, d2):
        """Called every time the worker thread successfully parses a packet."""
        self.time_data.append(t)
        self.data1_list.append(d1)
        self.data2_list.append(d2)

        # Update plots
        self.curve1.setData(self.time_data, self.data1_list)
        self.curve2.setData(self.time_data, self.data2_list)

        # Enforce 5-minute (300 seconds) span on the X-axis
        if t > 300:
            self.plot_widget.setXRange(t - 300, t)
        else:
            self.plot_widget.setXRange(0, 300)

    def handle_error(self, err_msg):
        QMessageBox.critical(self, "Serial Error", f"An error occurred:\n{err_msg}")
        self.disconnect_serial()

    def stop_and_save(self):
        """Disconnects (if connected) and saves all gathered data to a file."""
        self.disconnect_serial()
        
        if not self.time_data:
            QMessageBox.information(self, "Info", "No data to save.")
            return

        filename = f"Output/acquired_data_{time.strftime('%Y%m%d_%H%M%S')}.txt"
        try:
            with open(filename, 'w') as f:
                f.write("Timestamp(s)\tDelta_t(s)\tDATA1\tDATA2\n")
                
                # Calculate the time difference between consecutive points as requested
                for i in range(len(self.time_data)):
                    t = self.time_data[i]
                    d1 = self.data1_list[i]
                    d2 = self.data2_list[i]
                    
                    delta_t = 0.0 if i == 0 else (self.time_data[i] - self.time_data[i-1])
                    
                    f.write(f"{t:.4f}\t{delta_t:.6f}\t{d1}\t{d2}\n")
            
            QMessageBox.information(self, "Success", f"Data saved successfully to {filename}")
        except Exception as e:
            QMessageBox.critical(self, "Save Error", f"Could not save file:\n{e}")

# --- 3. Application Execution ---
if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec_())