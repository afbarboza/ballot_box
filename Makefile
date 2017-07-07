all:
	gcc data.c cJSON.c json.c server.c -o server -Wall -lpthread -g
	gcc client.c data.c -o client -Wall -g

debug:
	gcc data.c cJSON.c json.c server.c -o server -Wall -lpthread -DDEBUG -g
	gcc client.c data.c -o client -Wall

clear:
	rm client server
