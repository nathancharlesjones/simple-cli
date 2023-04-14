from PyQt5 import QtWidgets, QtCore, QtGui
from pyqtgraph import PlotWidget, plot
import pyqtgraph as pg
import time
import math
import sys
import numpy as np
import serial

class MainWindow(QtWidgets.QMainWindow):

	def __init__(self, *args, **kwargs):
		super(MainWindow, self).__init__(*args, **kwargs)

		# Object variables/constants
		self.window_width_s = 5
		self.sampling_freq_Hz = 100
		self.buffer_size = int(self.window_width_s*self.sampling_freq_Hz)

		# Create serial object
		self.com_port = "COM6"
		self.baud_rate = 115200
		self.cli_device = serial.Serial(self.com_port, self.baud_rate, timeout=0.5)

		# Create main graph
		self.graphWidget = pg.PlotWidget()
		self.graphWidget.setLabel('left', 'ADC values')
		self.graphWidget.setLabel('bottom', 'Time (sec)')
		self.graphWidget.setBackground('w')
		
		# Set up the main window
		self.setWindowTitle('''Simple "CLI" to Read ADC Values''')
		self.setCentralWidget(self.graphWidget)

		# Create array for ADC values
		self.time_vals = list(np.linspace(-self.window_width_s,0,self.buffer_size))
		self.adc_values = [0] * self.buffer_size
		self.green_pen = pg.mkPen(color=(0, 255, 0))
		self.adc_line =  self.graphWidget.plot(self.time_vals, self.adc_values, name="ADC values", pen=self.green_pen)

		self.timer = QtCore.QTimer()
		self.timer.setInterval(int(1000/self.sampling_freq_Hz))
		self.timer.timeout.connect(self.update_plot_data)
		self.timer.start()

	def update_plot_data(self):
		try:
			self.cli_device.write("r\n".encode())
			newVal = int(self.cli_device.readline().strip().decode('UTF-8'))
			#print(newVal)

			self.adc_values = self.adc_values[1:]  # Remove the first
			self.adc_values.append(newVal)  # Add a new value.
			self.adc_line.setData(self.time_vals, self.adc_values)  # Update the data.
		except:
			pass


app = QtWidgets.QApplication(sys.argv)
w = MainWindow()
w.show()
sys.exit(app.exec_())