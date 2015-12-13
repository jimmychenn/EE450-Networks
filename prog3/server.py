import socket
import select

UDP_IP = "192.168.0.4"
UDP_PORT = 4040

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))
#sock.setblocking(0)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 64000)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 64000)

ready = select.select([sock], [], [])
ID = 1
while True:
	if ready[0]:
		data, addr = sock.recvfrom(64000) # buffer size is 64 kilobytes
		print "received message:",data.split()[0]
		#print "address:", addr[0], addr[1]
		if (int(data.split()[0][3:]) == ID ):
			REPLY = data.replace("ID","ACK")
			sock.sendto(REPLY,(addr[0],addr[1]))
			print "replied with ID:",REPLY.split()[0]
			ID += 1
		elif (int(data.split()[0][3:]) < ID ):
			ID = int(data.split()[0][3:])
			REPLY = data.replace("ID","ACK")
			sock.sendto(REPLY,(addr[0],addr[1]))
			print "replied with ID:",REPLY.split()[0]
			ID += 1