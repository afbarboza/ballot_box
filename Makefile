all:
	gcc server.c data.c -o server -Wall -lpthread
	gcc client.c data.c -o client -Wall

clear:
	rm client server
