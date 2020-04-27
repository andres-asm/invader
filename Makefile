DEBUG := 0
GIT_VERSION := " $(shell git rev-parse --short HEAD)"

INCLUDE	:=
SOURCES_C   :=
SOURCES_CXX :=

CFLAGS   :=
CXXFLAGS :=
LIBS  :=

WITH_GUI := imgui

ifeq ($(WITH_GUI), nuklear)
   TARGET = invader_nuklear_sdl2
   WITH_GUI := nuklear
else ifeq ($(WITH_GUI), imgui)
   TARGET = invader_imgui_sdl2
   WITH_GUI := imgui
endif

include Makefile.common

OBJDIR = obj/

OBJECTS  = $(SOURCES_CXX:.cpp=.o) $(SOURCES_C:.c=.o)
LOCALIZATION = $(SOURCES_LOCALIZATION:.c=.po)

ifeq ($(DEBUG),1)
ifneq ($(OS),Windows_NT)
   CFLAGS += -g -O0 -DDEBUG -pedantic -D_GNU_SOURCE
   CXXFLAGS += -g -O0 -DDEBUG
   LIBS += -g
else
   CFLAGS += -g -O0 -DDEBUG -pedantic
   CXXFLAGS += -g -O0 -DDEBUG
endif
else
ifneq ($(OS),Windows_NT)
   CFLAGS += -O3 -pedantic -D_GNU_SOURCE
   CXXFLAGS += -O3 -pedantic -DGNU_SOURCE
else
   CFLAGS += -O3 -pedantic -DGNU_SOURCE
   CXXFLAGS += -O3 -pedantic -DGNU_SOURCE
endif
endif

ifneq ($(SANITIZER),)
   CFLAGS += -fsanitize=$(SANITIZER) $(CFLAGS)
   LIBS += -fsanitize=$(SANITIZER) $(LIBS)
endif

OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
   TARGET := $(TARGET).exe
   LIBS += -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lm -lGLU32 -lGLEW32 -lintl
else
   UNAME_S := $(shell uname -s)
   ifeq ($(UNAME_S),Darwin)
      LIBS += -lSDL2 -framework OpenGL -lm -lGLEW
   else
      LIBS += -lSDL2 -lGL -lm -lGLU -lGLEW -ldl
   endif
endif

all: $(TARGET)
$(TARGET): $(OBJECTS) $(LOCALIZATION)

ifeq ($(STATIC_LINKING), 1)
	$(AR) rcs $@ $(OBJECTS)
else
	$(CXX) -o $@ $(OBJECTS) $(LIBS)
endif

%.po: %.c

	xgettext -k_ -j -lC --sort-output -o intl/invader.pot $^
	mkdir -p intl/en/LC_MESSAGES
	msgmerge --update intl/en/invader.po intl/invader.pot
	msgfmt --output-file=intl/en/LC_MESSAGES/invader.mo intl/en/invader.po

%.o: %.c
	$(CC) $(INCLUDE) $(DEFINES) $(CFLAGS) -c $^ -o $@

%.o: %.cpp
	$(CXX) $(INCLUDE) $(DEFINES) $(CXXFLAGS) -c $^ -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
	find ./intl -name *.mo -exec rm {} \;
	find ./intl -name *.po~ -exec rm {} \;

.PHONY: clean install uninstall
