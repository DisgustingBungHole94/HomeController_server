SRCDIR = src
STRUCTURE = $(shell cd $(SRCDIR) && find . -type d)

DEPSDIR = thirdparty

INCLUDES += -I$(DEPSDIR)/websocketpp/

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

# src root
_OBJECTS += homecontroller.o
_OBJECTS += main.o

# app
_OBJECTS += app/auth_manager.o
_OBJECTS += app/device_manager.o 
_OBJECTS += app/user_manager.o 
_OBJECTS += app/user.o

# device
_OBJECTS += device/device_server.o
_OBJECTS += device/device_session.o

# http/modules
_OBJECTS += http/modules/device_module.o
_OBJECTS += http/modules/login_module.o
_OBJECTS += http/modules/logout_module.o
_OBJECTS += http/modules/module.o
_OBJECTS += http/modules/register_device_module.o

# http
_OBJECTS += http/http_module_manager.o
_OBJECTS += http/http_server.o
_OBJECTS += http/http_session.o

# net
_OBJECTS += net/tls_server.o

# thread
_OBJECTS += thread/job.o
_OBJECTS += thread/thread_pool.o

# util
_OBJECTS += util/config.o
_OBJECTS += util/id.o
_OBJECTS += util/logger.o
_OBJECTS += util/string.o
_OBJECTS += util/timer.o

# ws
_OBJECTS += ws/ws_server.o
_OBJECTS += ws/ws_session.o

OBJECTS = $(patsubst %,$(OBJECTDIR)/%,$(_OBJECTS))

$(OBJECTDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJECTDIR)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(OBJECTDIR):
	mkdir -p $(OBJECTDIR)
	mkdir -p $(addprefix $(OBJECTDIR)/,$(STRUCTURE))

clean:
	rm -rf bin

.PHONY: clean