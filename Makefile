all:
	gcc data.c cJSON.c json.c server.c -o server -Wall -lpthread -g
	gcc client.c data.c -o client -Wall -g

debug:
	gcc data.c cJSON.c json.c server.c -o server -Wall -lpthread -DDEBUG -g
	gcc client.c data.c -o client -Wall

raise:
	scp -P 22200 *.c *.h Makefile g11bsi@cosmos.lasdpc.icmc.usp.br:.

log:
	ssh g11bsi@cosmos.lasdpc.icmc.usp.br -p 22200

clear:
	rm client server
