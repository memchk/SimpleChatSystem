SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

SRCEXT := cpp

SHARED := shared

CLIENT_EXECUTABLE := scs_client
CLIENT_TARGET := $(TARGETDIR)/$(CLIENT_EXECUTABLE)
CLIENT_SOURCES := $(shell find $(SRCDIR)/$(CLIENT_EXECUTABLE) $(SRCDIR)/$(SHARED) -type f -name *.$(SRCEXT))
CLIENT_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(CLIENT_SOURCES:.$(SRCEXT)=.o))

SERVER_EXECUTABLE := scs_server
SERVER_TARGET := $(TARGETDIR)/$(SERVER_EXECUTABLE)
SERVER_SOURCES := $(shell find $(SRCDIR)/$(SERVER_EXECUTABLE) $(SRCDIR)/$(SHARED) -type f -name *.$(SRCEXT))
SERVER_OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SERVER_SOURCES:.$(SRCEXT)=.o))


BUILDLIST := $(patsubst include/%,$(BUILDDIR)/%,$(INCDIRS))

CC := g++

INC := -I /usr/local/include -I ./include
LIB := -L /usr/local/lib
CFLAGS += -flto -std=c++17 -MMD -MP -g -Wall -Wextra -Werror=return-type
LINK_FLAGS := -flto -g

.PHONY: clean all

all : $(CLIENT_TARGET) $(SERVER_TARGET) ;

$(CLIENT_TARGET): $(CLIENT_OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo "Linking $(CLIENT_TARGET)"
	$(CC) $^ -o $(CLIENT_TARGET) $(LINK_FLAGS) $(LIB)

$(SERVER_TARGET): $(SERVER_OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo "Linking $(SERVER_TARGET)"
	$(CC) $^ -o $(SERVER_TARGET) $(LINK_FLAGS) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

-include $(CLIENT_OBJECTS:.o=.d)
-include $(SERVER_OBJECTS:.o=.d)

clean:
	@rm -rf bin build