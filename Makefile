

ifeq ($(WITH_GUI), nuklear)
   BIN = invader_nuklear_sdl2
   WITH_GUI := nuklear
else ifeq ($(WITH_GUI), imgui)
   BIN = invader_imgui_sdl2
   WITH_GUI := imgui
endif

include Makefile.common

# Flags
CFLAGS += -std=c99

ifeq ($(DEBUG),1)
ifneq ($(OS),Windows_NT)
   CFLAGS += -g -O0 -DDEBUG -pedantic -D_GNU_SOURCE
   LIBS += -g
else
   CFLAGS += -g -O0 -DDEBUG -pedantic
endif
else
ifneq ($(OS),Windows_NT)
   CFLAGS += -O3 -pedantic -D_GNU_SOURCE
else
   CFLAGS += -O3 -pedantic
endif
endif

ifneq ($(SANITIZER),)
   CFLAGS   := -fsanitize=$(SANITIZER) $(CFLAGS)
   LIBS  := -fsanitize=$(SANITIZER) $(LIBS)
endif

OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
   BIN := $(BIN).exe
   LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lm -lGLU32 -lGLEW32
else
   UNAME_S := $(shell uname -s)
   ifeq ($(UNAME_S),Darwin)
      LIBS = -lSDL2 -framework OpenGL -lm -lGLEW
   else
      LIBS = -lSDL2 -lGL -lm -lGLU -lGLEW -ldl
   endif
endif

$(BIN):
	@mkdir -p obj
	rm -f obj/$(BIN) $(OBJS)
	$(CC) $(SOURCES_C) $(CFLAGS) -o $(BIN) $(LIBS) $(INCLUDE)



