import PyQt5
import sys
from PyQt5.QtWidgets import QApplication, QMainWindow
from PyQt5.QtCore import QObject, pyqtSignal, QTimer
import socket
from client_ui import Ui_MainWindow

class Main(QMainWindow, Ui_MainWindow):
	def __init__(self, sock):
		super().__init__()
		self.setupUi(self)
		self.sock = sock
		self.send.clicked.connect(self.send_message)

	def show_message(self, message):
		self.chat.append(message)

	def send_message(self):
		message = self.enter.toPlainText()
		self.sock.send(message.encode())


class Chat(QObject):
	messageReceived = pyqtSignal(str)

	def __init__(self, sock):
		super().__init__()
		self.sock = sock
		QTimer.singleShot(1000, self.check_message)

	def check_message(self):
		try:
			message = self.sock.recv(2048).decode()
		except Exception:
			QTimer.singleShot(1000, self.check_message)
			return
		if len(message) > 0:
			self.messageReceived.emit(message)
			QTimer.singleShot(1000, self.check_message)


def main():
	host = "89.223.123.237"
	port = 5002
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	sock.connect((host, port))
	sock.setblocking(False)

	app = QApplication(sys.argv)
	main = Main(sock)
	main.show()
	c = Chat(sock)
	c.messageReceived.connect(main.show_message)
	sys.exit(app.exec())


if (__name__ == '__main__'):
	main()
