import socket
import select
import time

def packet(ack, packsize):
		pack = "ID:" + str(ack) + " DATA:"
		fill = ""
		fill = fill.zfill(packsize - len(pack))
		pack = pack + fill
		return pack


UDP_IP_CLIENT = "192.168.0.4"
UDP_IP_SERVER = "192.168.0.17"
UDP_PORT_SERVER = 4045
UDP_PORT_CLIENT = 4046

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP_CLIENT, UDP_PORT_CLIENT))
sock.setsockopt(socket.SOL_SOCKET, socket.SO_SNDBUF, 64000)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 64000)
sock.setblocking(0)

Results = {}
PACK_SIZE_Results = {}
LIST_MAXWINSIZE = range(0,9)
for i in range(0,len(LIST_MAXWINSIZE)):
	LIST_MAXWINSIZE[i] = pow(2,LIST_MAXWINSIZE[i])

LIST_PACK_SIZE = [32, 512, 1400, 8192]

for PACK_SIZE in LIST_PACK_SIZE:
	for MAXWINSIZE in LIST_MAXWINSIZE:
		LIST_LOST_PACK = []
		LIST_LOST_BYTES = []
		LIST_THROUGHPUT = []
		for trial in range(1,6):
			PACK_NUM = 0
			LOST_PACK = 0
			SEND_ID = 1
			REC_ID = 1
			MESSAGE = packet(SEND_ID, PACK_SIZE)
			WINSIZE = 1
			error = False
			MINUNACK = 0;
			exp_start = time.time()
			rtt_start = time.clock()
			while(time.time() - exp_start < 59):
				if(time.clock() - rtt_start > 0.01):
					rtt_start = time.clock()
					#print "HELLO"
					MINUNACK = 0		
					if(error):
						if(WINSIZE%2 == 1):
							WINSIZE = (WINSIZE+1)/2
						else:
							WINSIZE = WINSIZE/2
						#print "WINDOW SIZE DECREASED:", WINSIZE
					else:
						if(WINSIZE < MAXWINSIZE):
							WINSIZE += 1
							#print "WINDOW SIZE INCREASED:", WINSIZE
					error = False
				else:
					while(MINUNACK<WINSIZE):
						#print "Sending packet with ID: ", SEND_ID
						sock.sendto(MESSAGE, (UDP_IP_SERVER, UDP_PORT_SERVER))
						SEND_ID += 1
						MESSAGE = packet(SEND_ID,PACK_SIZE)
						MINUNACK += 1
						PACK_NUM += 1

					for x in range(0,MINUNACK):
						ready = select.select([sock], [], [], 0.05)
						if ready[0]:
							data, addr = sock.recvfrom(64000) # buffer size is 64 kilobytes
							#print "server replied:",data.split()[0]
							if(data.split()[0][4:] == str(REC_ID)):
								#print "ACK received"
								error = False
								REC_ID += 1
								MINUNACK -= 1
						else:
							#print "SEND_ID: ", SEND_ID, " REC_ID: ", REC_ID
							#print "MINUNACK: ",MINUNACK, "WINSIZE: ", WINSIZE
							SEND_ID = REC_ID
							MESSAGE = packet(SEND_ID, PACK_SIZE)
							LOST_PACK += 1
							error = True
							break
						ready = []

			ack_start = time.clock()
			for x in range(0,MINUNACK):
						ready = select.select([sock], [], [], 0.05)
						if ready[0]:
							data, addr = sock.recvfrom(64000) # buffer size is 64 kilobytes
							#print "server replied:",data.split()[0]
							if(data.split()[0][4:] == str(REC_ID)):
								print "ACK received"
			ack_end = time.clock()
			ack_time = ack_end - ack_start
			print "Packet size: ", PACK_SIZE
			print "  Test done with window size: ", MAXWINSIZE
			print "    Trial run number", trial
			THROUGHPUT = float(PACK_NUM-LOST_PACK)/float(PACK_NUM)
			LOST_BYTES = LOST_PACK*PACK_SIZE
			#print "      Time taken to ACK packets: ", ack_time
			print "      Throughput: ", THROUGHPUT
			print "      Packets lost: ", LOST_PACK
			print "      Bytes lost: ", LOST_BYTES

			LIST_THROUGHPUT.append(THROUGHPUT)
			LIST_LOST_PACK.append(LOST_PACK)
			LIST_LOST_BYTES.append(LOST_BYTES)
		PACK_SIZE_Results[MAXWINSIZE] = [LIST_THROUGHPUT, LIST_LOST_PACK, LIST_LOST_BYTES]
		print PACK_SIZE_Results
	print "PACK_SIZE: ", PACK_SIZE
	Results[PACK_SIZE] = PACK_SIZE_Results
	PACK_SIZE_Results = {}
	print "PACK_SIZE FINISHED"
	print Results