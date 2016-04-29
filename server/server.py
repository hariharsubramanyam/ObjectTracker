# Simple TCP server to connect CV Object Tracking to iOS client

from twisted.internet.protocol import Protocol, Factory
from twisted.internet import reactor, task
import zmq


class serverProtocol(Protocol):
	def connectionMade(self):
		self.factory.clients.append(self)

	def connectionLost(self, reason):
	    self.factory.clients.remove(self)

	def message(self, message):
		self.transport.write(message + '\n')


class customFactory(Factory):
	protocol = serverProtocol

	def __init__(self):
		self.clients = []
		self.lc = task.LoopingCall(self.announce)
		self.lc.start(0.01)

	def announce(self):
		# socket to connect to CV object tracking
		context = zmq.Context()
		socket = context.socket(zmq.SUB)
		socket.setsockopt(zmq.SUBSCRIBE, "")
		socket.connect ("tcp://localhost:%s" % 5555)

		data = socket.recv()

		for client in self.clients:
			client.transport.write(data);

		print data


reactor.listenTCP(80, customFactory())
print "Server started."
reactor.run()
