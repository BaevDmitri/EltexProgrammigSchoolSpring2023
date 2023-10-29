LOCAL = AF_LOCAL/
IPv4 = AF_INET/
UDP = udp_
TCP = tcp_
CLIENT = client
SERVER = server

.PHONY: help tcp_local udp_local tcp_inet udp_inet remove

help:
	@echo tcp_local - server and client for family AF_LOCAL and TCP-socket type
	@echo udp_local - server and client for family AF_LOCAL and UDP-socket type
	@echo tcp_inet - server and client for family AF_INET and TCP-socket type
	@echo udp_inet - server and client for family AF_INET and UDP-socket type

tcp_local:
	gcc $(LOCAL)$(TCP)*.c -c 
	gcc $(TCP)$(CLIENT).o -o $(TCP)$(CLIENT)
	gcc $(TCP)$(SERVER).o -o $(TCP)$(SERVER) 

udp_local:
	gcc $(LOCAL)$(UDP)*.c -c
	gcc $(UDP)$(CLIENT).o -o $(UDP)$(CLIENT)
	gcc $(UDP)$(SERVER).o -o $(UDP)$(SERVER)

tcp_inet:
	gcc $(IPv4)$(TCP)*.c -c
	gcc $(TCP)$(CLIENT).o -o $(TCP)$(CLIENT)
	gcc $(TCP)$(SERVER).o -o $(TCP)$(SERVER)

udp_inet:
	gcc $(IPv4)$(UDP)*.c -c
	gcc $(UDP)$(CLIENT).o -o $(UDP)$(CLIENT)
	gcc $(UDP)$(SERVER).o -o $(UDP)$(SERVER)

remove:
	rm *.o
