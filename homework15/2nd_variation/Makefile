SERVER = server
CLIENT = client
FLAGS = -lpthread

.PHONY: all clean client server

help: ## Show this help
	@fgrep -h "##" $(MAKEFILE_LIST) | fgrep -v fgrep | sed -e 's/\\$$//' | sed -e 's/##//'

all: ## Make server and client 
	$(SERVER) $(CLIENT)

clean: ## Clean *.o-files
	rm *.o

server: ## Make server
	gcc $(SERVER).c -c $(FLAGS)
	gcc $(SERVER).o -o $(SERVER) $(FLAGS)

client: ## Make client 
	gcc $(CLIENT).c -c $(FLAGS)
	gcc $(CLIENT).o -o $(CLIENT) $(FLAGS)
