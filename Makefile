SRCDIR = src
STRUCTURE = $(shell find $(SRCDIR) -type d)

DEPSDIR = thirdparty

INCLUDES += -I$(DEPSDIR)/websocketpp/
INCLUDES += -I$(DEPSDIR)/rapidjson/include/

CXX ?= g++
CXXFLAGS ?= -g $(INCLUDES)

BINARYDIR = bin
OBJECTDIR = $(BINARYDIR)/obj

TARGET = $(BINARYDIR)/homecontroller

LIBS += -lllhttp
LIBS += -lpthread
LIBS += -lssl
LIBS += -lcrypto
LIBS += -lboost_system
LIBS += -lboost_random

SOURCES := $(shell find $(SRCDIR) -name '*.cpp')

OBJECTS := $(addprefix $(OBJECTDIR)/,$(SOURCES:%.cpp=%.o))

$(OBJECTDIR)/%.o: | $(OBJECTDIR)
	$(CXX) -c -o $@ $*.cpp $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

clean:
	rm -rf bin

.PHONY: clean