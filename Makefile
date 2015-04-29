SRC_DIRS=\
src/utils \
src/search 

MKDIR = mkdir -p
LOG_DIR = CounterExamples

all: multimake dirs

dirs:
	$(MKDIR) $(LOG_DIR)

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
