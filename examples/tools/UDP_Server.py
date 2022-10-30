#!/usr/bin/env python3

import socket

IP     		= "0.0.0.0"
PORT   		= 5246
BUFFERSIZE  = 1024

UDPServerSocket = socket.socket(family = socket.AF_INET, type = socket.SOCK_DGRAM)
UDPServerSocket.bind((IP, PORT))

if(__name__ == "__main__"):
	print("UDP server up and listening...")
	while(True):
		Received = UDPServerSocket.recvfrom(BUFFERSIZE)

		print("Message from Client: {}".format(Received[0]))
		print("Client IP Address: {}".format(Received[1]))

		UDPServerSocket.sendto(str.encode("Here I am!"), Received[1])