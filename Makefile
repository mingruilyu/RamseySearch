SRC_DIRS=\
src/utils \
src/search \

all: multimake 

multimake:
	@for d in $(SRC_DIRS);  \
	do          \
		make -C $$d;    \
	done;       \

clean:
	@for d in $(SRC_DIRS); \
	do          \
		make -C $$d clean;  \
	done;           \
