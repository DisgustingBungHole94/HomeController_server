SRCDIR=src

IDIRS = -I./websocketpp -I./rapidjson/include -I./llhttp/build
LDIR = lib

CXX=g++
CXXFLAGS=-g $(IDIRS) -L$(LDIR) -Wl,-rpath,./$(LDIR)

TARGET=bin/homecontroller
ODIR=bin/obj

LIBS=-lllhttp -lpthread -lssl -lcrypto -lboost_system -lboost_random

_DEPS =	homecontroller.h \
		util/logger.h util/exception/general_exception.h util/exception/socket_close_exception.h util/string.h util/config.h util/timer.h util/id.h \
		net/tls_server.h \
		http/http_response.h http/http_session.h http/http_module_manager.h http/http_server.h \
		http/modules/module.h http/modules/login_module.h http/modules/logout_module.h http/modules/register_device_module.h http/modules/device_module.h \
		app/auth_manager.h app/user_manager.h app/device_manager.h app/session.h app/user.h app/device.h \
		ws/ws_session.h ws/ws_server.h ws/ws_server_config.h \
		thread/thread_pool.h thread/job.h \
		device/device_server.h device/device_session.h
DEPS = $(patsubst %,$(SRCDIR)/%,$(_DEPS))

_OBJ = 	main.o homecontroller.o \
		util/logger.o util/string.o util/config.o util/timer.o util/id.o \
		net/tls_server.o \
		http/http_session.o http/http_module_manager.o http/http_server.o \
		http/modules/module.o http/modules/login_module.o http/modules/logout_module.o http/modules/register_device_module.o http/modules/device_module.o \
		app/auth_manager.o app/user_manager.o app/device_manager.o app/user.o \
		ws/ws_session.o ws/ws_server.o \
		thread/thread_pool.o thread/job.o \
		device/device_server.o device/device_session.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS) | bin
	$(CXX) -c -o $@ $< $(CXXFLAGS)

$(TARGET): llhttp/build/libllhttp.so $(OBJ) 
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

bin:
	mkdir $@
	mkdir $@/obj
	mkdir $@/obj/app
	mkdir $@/obj/device
	mkdir $@/obj/http
	mkdir $@/obj/http/modules
	mkdir $@/obj/net
	mkdir $@/obj/thread
	mkdir $@/obj/util
	mkdir $@/obj/ws

llhttp/build/libllhttp.so:
	cd llhttp && npm install
	cd llhttp && make
	mkdir lib
	cd lib && ln -s ../llhttp/build/libllhttp.so libllhttp.so

.PHONY: clean cleandeps

clean:
	find $(ODIR) -type f -name '*.o' -delete
	rm -f $(TARGET)

cleanall: clean
	rm -r bin
	rm -r lib
	cd llhttp && make clean