SUB_DIR =\
SockServer \
SockClient

all: RECURSIVE_MAKE

RECURSIVE_MAKE:
	@for subdir in $(SUB_DIR); \
	do \
		make -C $$subdir; \
	done;

clean:
	@for subdir in $(SUB_DIR); \
	do \
		make -C $$subdir clean; \
	done;
