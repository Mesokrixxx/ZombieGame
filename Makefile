NAME = ZombieGame

W64DEVKIT_PATH = C:/w64devkit

ifeq ($(OS), Windows_NT)
	BIN = $(NAME).exe
	CC = $(W64DEVKIT_PATH)/bin/gcc.exe
	CFLAGS = -I$(W64DEVKIT_PATH)/include
	LFLAGS = -L$(W64DEVKIT_PATH)/lib -lSDL2 -lglew32 -lopengl32
else
	BIN = $(NAME)
	UNAME_S = $(shell uname -s)
	ifeq (UNAME_S, Linux)
		CC = clang
		CFLAGS = 
		LFLAGS = -lSDL2 -lGLEW -lGL
	endif
	ifeq (UNAME_S, Darwin)
		CC = 
		CFLAGS = 
		LFLAGS = 
	endif
endif

CLEAN_METHOD = rm -f

SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(BIN) $(LFLAGS)

clean:
	$(CLEAN_METHOD) $(OBJS) $(BIN)

re: clean all

.PHONY: all $(NAME) clean run
