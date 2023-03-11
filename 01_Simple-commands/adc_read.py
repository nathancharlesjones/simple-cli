from PyQt5 import QtWidgets, QtCore, QtGui
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import time
import math
import sys
import numpy as np
import serial

count = 0

class MainWindow(QtWidgets.QMainWindow):

	def __init__(self, *args, **kwargs):
		super(MainWindow, self).__init__(*args, **kwargs)

		# Object variables/constants
		self.com_port = "COM6"
		self.baud_rate = 115200
		self.window_width_s = 5
		self.sampling_freq_Hz = 10
		self.buffer_size = int(self.window_width_s*self.sampling_freq_Hz)
		self.adc_values_omega = 1
		self.adc_values_offset = 0

		# Create serial object
		self.cli_device = serial.Serial(self.com_port, self.baud_rate)

		# Flush input buffer?

		# Create main graph
		self.graphWidget = pg.PlotWidget()
		self.graphWidget.setLabel('left', 'ADC values')
		self.graphWidget.setLabel('bottom', 'Time (sec)')
		self.graphWidget.setBackground('w')
		self.graphWidget.addLegend()

		# Create start/stop logging button
		self.log_start_stop_button = QtWidgets.QPushButton("Start ADC readings", self)
		self.log_start_stop_button.setCheckable(True)
		self.log_start_stop_button.clicked.connect(self.startStopAdc)

		# Combine graph and button into a single widget
		self.graph_and_toolbar_layout = QtWidgets.QVBoxLayout()
		self.graph_and_toolbar_layout.addWidget(self.graphWidget)
		self.graph_and_toolbar_layout.addWidget(self.log_start_stop_button)

		# Set up the main window
		self.setWindowTitle('''Simple "CLI" to Read ADC Values''')
		self.main_widget = QtWidgets.QWidget()
		self.main_widget.setLayout(self.graph_and_toolbar_layout)
		self.setCentralWidget(self.main_widget)

		# Create array for ADC values
		self.count_vals = list(np.linspace(-self.window_width_s,0,self.buffer_size))
		self.adc_values = [0] * self.buffer_size
		self.green_pen = pg.mkPen(color=(0, 255, 0))
		self.adc_line =  self.graphWidget.plot(self.count_vals, self.adc_values, name="ADC values", pen=self.green_pen)

		self.timer = QtCore.QTimer()
		self.timer.setInterval(int(1000/self.sampling_freq_Hz))
		self.timer.timeout.connect(self.update_plot_data)
		self.timer.start()

	def startStopAdc(self):
		if self.log_start_stop_button.isChecked():
			self.cli_device.write("on\n".encode())
		else:
			self.cli_device.write("off\n".encode())
		return

	def update_plot_data(self):
		while self.cli_device.in_waiting > 0:
			self.adc_values = self.adc_values[1:]  # Remove the first

			newVal = int(self.cli_device.readline().strip().decode('UTF-8'))
			print(newVal)

			self.adc_values.append(newVal)  # Add a new value.
			self.adc_line.setData(self.count_vals, self.adc_values)  # Update the data.

app = QtWidgets.QApplication(sys.argv)
w = MainWindow()
w.show()
sys.exit(app.exec_())