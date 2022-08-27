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
LIBS += -lboost_system
LIBS += -lboost_random
LIBS += -lmongocxx
LIBS += -lbsoncxx
LIBS += -lhomecontroller

# src root
_OBJECTS += homecontroller.o
_HEADERS += homecontroller.h

_OBJECTS += main.o

# app
#_OBJECTS += app/auth_manager.o
#_HEADERS += app/auth_manager.h

#_OBJECTS += app/device_manager.o 
#_HEADERS += app/device_manager.h

#_HEADERS += app/device.h

#_HEADERS += app/session.h

#_OBJECTS += app/user_manager.o
#_HEADERS += app/user_manager.h

#_OBJECTS += app/user.o
#_HEADERS += app/user.h

_OBJECTS += app/session.o
_HEADERS += app/session.h

_OBJECTS += app/handler.o
_HEADERS += app/handler.h

# device/handlers
#_OBJECTS += device/handlers/handler.o
#_HEADERS += device/handlers/handler.h

#_OBJECTS += device/handlers/lightstrip_handler.o
#_HEADERS += device/handlers/lightstrip_handler.h

# device
#_OBJECTS += device/device_handler_manager.o
#_HEADERS += device/device_handler_manager.h

#_OBJECTS += device/device_server.o
#_HEADERS += device/device_server.h

#_OBJECTS += device/device_session.o
#_HEADERS += device/device_session.h

# http/modules
#_OBJECTS += http/modules/device_module.o
#_HEADERS += http/modules/device_module.h

#_OBJECTS += http/modules/login_module.o
#_HEADERS += http/modules/login_module.h

#_OBJECTS += http/modules/logout_module.o
#_HEADERS += http/modules/logout_module.h

#_OBJECTS += http/modules/module.o
#_HEADERS += http/modules/module.h

#_OBJECTS += http/modules/register_device_module.o
#_HEADERS += http/modules/register_device_module.h

# http
#_OBJECTS += http/http_module_manager.o
#_HEADERS += http/http_module_manager.h

#_HEADERS += http/http_response.h

#_OBJECTS += http/http_server.o
#_HEADERS += http/http_server.h

#_OBJECTS += http/http_session.o
#_HEADERS += http/http_session.h

_OBJECTS += http/http_handler.o
_HEADERS += http/http_handler.h

# net
#_OBJECTS += net/tls_server.o
#_HEADERS += net/tls_server.h

#_OBJECTS += net/mongodb_pool.o
#_HEADERS += net/mongodb_pool.h

# thread
#_OBJECTS += thread/job.o
#_HEADERS += thread/job.h

#_OBJECTS += thread/thread_pool.o
#_HEADERS += thread/thread_pool.h

# util
#_OBJECTS += util/config.o
#_HEADERS += util/config.h

#_OBJECTS += util/id.o
#_HEADERS += util/id.h

#_OBJECTS += util/json.o
#_HEADERS += util/json.h

#_OBJECTS += util/logger.o
#_HEADERS += util/logger.h

#_OBJECTS += util/string.o
#_HEADERS += util/string.h

#_OBJECTS += util/timer.o
#_HEADERS += util/timer.h

# ws
#_HEADERS += ws/ws_server_config.h

#_OBJECTS += ws/ws_server.o
#_HEADERS += ws/ws_server.h

#_OBJECTS += ws/ws_session.o
#_HEADERS += ws/ws_session.h


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