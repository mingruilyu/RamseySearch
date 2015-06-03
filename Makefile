SERVER_SRC_DIRS=\
src/utils \
src/server

CLIENT_SRC_DIRS=\
src/utils \
src/search \
src/client \
 
SRC_DIRS=\
src/utils \
src/search \
src/server \
src/client

LOG_DIRS=\
src/log

all: multimake_server

server: multimake_server

client: multimake_client

log: multimake_log

multimake_log:
	@for d in $(LOG_DIRS);  \
	do          \
		make -C $$d;    \
	done;       \

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
