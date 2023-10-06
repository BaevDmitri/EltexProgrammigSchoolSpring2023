SERVER = chat_server
CLIENT = chat_client

.PHONY: all clean client server

all: $(SERVER) $(CLIENT)

client: $(CLIENT)

server: $(SERVER)

clean:
	rm *.o

chat_server.o: chat_server.c
	gcc chat_server.c -c chat_server.o -lrt -lpthread -lncurses

chat_client.o: chat_client.c
	gcc chat_client.c -c chat_client.o -lncurses -lrt -lpthread

chat_functions.o: chat_functions.c
	gcc chat_functions.c -c chat_functions.o -lncurses -lrt -lpthread

$(SERVER): chat_server.o chat_functions.o chat_header.h
	gcc chat_server.o chat_functions.o -o $(SERVER) -lrt -lpthread -lncurses

$(CLIENT): chat_client.o chat_functions.o chat_header.h
	gcc chat_client.o chat_functions.o -o $(CLIENT) -lrt -lpthread -lncurses