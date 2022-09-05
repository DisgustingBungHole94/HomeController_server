SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

DEPSDIR = thirdparty

INCLUDES += -I$(DEPSDIR)/websocketpp/
INCLUDES += -I/usr/local/include/mongocxx/v_noabi
INCLUDES += -I/usr/local/include/bsoncxx/v_noabi

CXX ?= g++
CXXFLAGS ?= -g $(INCLUDES)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGET = $(BINARYDIR)/homecontroller

LIBS += -lllhttp
LIBS += -lpthread
LIBS += -lssl
LIBS += -lcrypto
LIBS += -lhomecontroller

# src root
_OBJECTS += homecontroller.o
_HEADERS += homecontroller.h

_OBJECTS += main.o

# app

_OBJECTS += app/session.o
_HEADERS += app/session.h

# http

_OBJECTS += http/http_session.o
_HEADERS += http/http_session.h

# net

_OBJECTS += net/hc_server.o
_HEADERS += net/hc_server.h
 
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