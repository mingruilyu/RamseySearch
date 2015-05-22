SERVER_SRC_DIRS=\
src/utils \
src/search \
src/communication \
src/server

CLIENT_SRC_DIRS=\
src/utils \
src/search \
src/communication \
src/client
 
all: multimake_server

server: multimake_server

client: multimake_client
 
multimake_client:
	@for d in $(CLIENT_SRC_DIRS);  \
	do          \
		make -C $$d;    \
	done;       \

multimake_server:
	@for d in $(SERVER_SRC_DIRS);  \
	do          \
		make -C $$d;    \
	done;       \

clean:
	@for d in $(SRC_DIRS); \
	do          \
		make -C $$d clean;  \
	done;           \
