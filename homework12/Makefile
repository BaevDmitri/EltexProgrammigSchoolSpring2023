SERVER = chat_server
CLIENT = chat_client
FLAGS = -lrt -lpthread -lncurses

.PHONY: all clean client server

all: $(SERVER) $(CLIENT)

client: $(CLIENT)

server: $(SERVER)

clean:
	rm *.o

chat_server.o: $(SERVER).c
	gcc $(SERVER).c -c $(FLAGS)

chat_client.o: $(CLIENT).c
	gcc $(CLIENT).c -c $(FLAGS)

chat_functions.o: chat_functions.c
	gcc chat_functions.c -c $(FLAGS)

$(SERVER): $(SERVER).o chat_functions.o chat_header.h
	gcc $(SERVER).o chat_functions.o -o $(SERVER) $(FLAGS)

$(CLIENT): $(CLIENT).o chat_functions.o chat_header.h
	gcc $(CLIENT).o chat_functions.o -o $(CLIENT) $(FLAGS)


