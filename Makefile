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
_HEADERS += homecontroller.h

_OBJECTS += main.o

# app
_OBJECTS += app/auth_manager.o
_HEADERS += app/auth_manager.h

_OBJECTS += app/device_manager.o 
_HEADERS += app/device_manager.h

_OBJECTS += app/user_manager.o
_HEADERS += app/user_manager.h

_OBJECTS += app/user.o
_HEADERS += app/user.h

# device
_OBJECTS += device/device_server.o
_HEADERS += device/device_server.h

_OBJECTS += device/device_session.o
_HEADERS += device/device_session.h

# http/modules
_OBJECTS += http/modules/device_module.o
_HEADERS += http/modules/device_module.h

_OBJECTS += http/modules/login_module.o
_HEADERS += http/modules/login_module.h

_OBJECTS += http/modules/logout_module.o
_HEADERS += http/modules/logout_module.h

_OBJECTS += http/modules/module.o
_HEADERS += http/modules/module.h

_OBJECTS += http/modules/register_device_module.o
_HEADERS += http/modules/register_device_module.h

# http
_OBJECTS += http/http_module_manager.o
_HEADERS += http/http_module_manager.h

_OBJECTS += http/http_server.o
_HEADERS += http/http_server.h

_OBJECTS += http/http_session.o
_HEADERS += http/http_session.h

# net
_OBJECTS += net/tls_server.o
_HEADERS += net/tls_server.h

# thread
_OBJECTS += thread/job.o
_HEADERS += thread/job.h

_OBJECTS += thread/thread_pool.o
_HEADERS += thread/thread_pool.h

# util
_OBJECTS += util/config.o
_HEADERS += util/config.h

_OBJECTS += util/id.o
_HEADERS += util/id.h

_OBJECTS += util/logger.o
_HEADERS += util/logger.h

_OBJECTS += util/string.o
_HEADERS += util/string.h

_OBJECTS += util/timer.o
_HEADERS += util/timer.h

# ws
_OBJECTS += ws/ws_server.o
_HEADERS += ws/ws_server.h

_OBJECTS += ws/ws_session.o
_HEADERS += ws/ws_session.h


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