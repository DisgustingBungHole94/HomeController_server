SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

DEPSDIR = thirdparty

CXX ?= g++
CXXFLAGS ?= -g $(INCLUDES)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGET = $(BINARYDIR)/homecontroller

LIBS += -lllhttp
LIBS += -lpthread
LIBS += -lssl
LIBS += -lhomecontroller

# src root
_OBJECTS += homecontroller.o
_HEADERS += homecontroller.h

_OBJECTS += main.o

# app

_OBJECTS += app/handler.o
_HEADERS += app/handler.h

_OBJECTS += app/session.o
_HEADERS += app/session.h

# http

_OBJECTS += http/http_handler.o
_HEADERS += http/http_handler.h

# net

_OBJECTS += net/hc_server.o
_HEADERS += net/hc_server.h
 
# ws

_OBJECTS += ws/ws_handler.o
_HEADERS += ws/ws_handler.h

OBJECTS = $(patsubst %,$(OBJECTDIR)/%,$(_OBJECTS))
HEADERS = $(patsubst %,$(SRCDIR)/%,$(_HEADERS))

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS) | $(OBJECTDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

clean:
	rm -rf bin

.PHONY: clean