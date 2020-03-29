DEBUG := 0
GIT_VERSION := " $(shell git rev-parse --short HEAD)"

INCLUDE	:=
SOURCES_C   :=
SOURCES_CXX :=

CFLAGS   := -std=c99
CXXFLAGS :=
LIBS  :=

ifeq ($(WITH_GUI), nuklear)
   TARGET = invader_nuklear_sdl2
   WITH_GUI := nuklear
else ifeq ($(WITH_GUI), imgui)
   TARGET = invader_imgui_sdl2
   WITH_GUI := imgui
endif

include Makefile.common

OBJECTS  = $(SOURCES_CXX:.cpp=.o) $(SOURCES_C:.c=.o)

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
   CFLAGS += -fsanitize=$(SANITIZER) $(CFLAGS)
   LIBS += -fsanitize=$(SANITIZER) $(LIBS)
endif

OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
   TARGET := $(TARGET).exe
   LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lm -lGLU32 -lGLEW32
else
   UNAME_S := $(shell uname -s)
   ifeq ($(UNAME_S),Darwin)
      LIBS = -lSDL2 -framework OpenGL -lm -lGLEW
   else
      LIBS = -lSDL2 -lGL -lm -lGLU -lGLEW -ldl
   endif
endif

all: $(TARGET)
$(TARGET): $(OBJECTS)

ifeq ($(STATIC_LINKING), 1)
   $(AR) rcs $@ $(OBJECTS)
else
	$(CXX) -o $@ $(OBJECTS) $(LIBS)
endif

%.o: %.c
   $(CC) $(INCLUDE) $(CFLAGS) -c $^ -o $@

%.o: %.cpp
   $(CXX) $(INCLUDE) $(CXXFLAGS) -c $^ -o $@

clean:
   rm -f $(OBJECTS) $(TARGET)

.PHONY: clean install uninstall
