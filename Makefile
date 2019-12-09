# This makefile was tested on Ubuntu 18.04 and assumes that std::thread is
# implemented using POSIX threads. Until I have a true automake, you'll have
# to adjust this file for your specific environment.

CXX ?= g++
LUA_VERSION ?= 5.1

default:
	$(CXX) -std=c++17 -O2 -I/usr/include/libxml2 -I/usr/include/lua$(LUA_VERSION) src/core/*.cpp src/core/exception/*.cpp src/core/parser/*.cpp src/core/parser/parsers/*.cpp src/core/instantiator/*.cpp src/core/instantiator/instantiators/*.cpp src/core/lua/*.cpp src/core/lua/api/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/standalone/*.cpp src/standalone/actions/*.cpp -o trogdor -lxml2 -pthread -llua$(LUA_VERSION)

debug:
	$(CXX) -std=c++17 -I/usr/include/libxml2 -I/usr/include/lua$(LUA_VERSION) -g src/core/*.cpp src/core/exception/*.cpp src/core/parser/*.cpp src/core/parser/parsers/*.cpp src/core/instantiator/*.cpp src/core/instantiator/instantiators/*.cpp src/core/lua/*.cpp src/core/lua/api/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/standalone/*.cpp src/standalone/actions/*.cpp -o trogdor -pthread -lxml2 -llua$(LUA_VERSION)

server:
	$(CXX) -DPTHREAD -std=c++17 -O2 -I/usr/include/libxml2 -I/usr/include/lua$(LUA_VERSION) src/core/*.cpp src/core/exception/*.cpp src/core/parser/*.cpp src/core/parser/parsers/*.cpp src/core/instantiator/*.cpp src/core/instantiator/instantiators/*.cpp src/core/lua/*.cpp src/core/lua/api/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/online/server/*.cpp src/online/server/network/*.cpp src/online/server/command/*.cpp src/online/server/command/actions/*.cpp src/online/server/iostream/*.cpp -o trogdor-server -pthread -lboost_system -lxml2 -llua$(LUA_VERSION)

client:
	echo "NOT YET IMPLEMENTED"

client-test:
	$(CXX) -DPTHREAD -std=c++17 -O2 -I/usr/include/libxml2 -I/usr/include/lua$(LUA_VERSION) src/online/client/test.cpp -oclient-test -lboost_system -pthread

server-debug:
	$(CXX) -DPTHREAD -std=c++17 -I/usr/include/libxml2 -I/usr/include/lua$(LUA_VERSION) -g src/core/*.cpp src/core/exception/*.cpp src/core/parser/*.cpp src/core/parser/parsers/*.cpp src/core/instantiator/*.cpp src/core/instantiator/instantiators/*.cpp src/core/lua/*.cpp src/core/lua/api/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/online/server/*.cpp src/online/server/network/*.cpp src/online/server/command/*.cpp src/online/server/command/actions/*.cpp src/online/server/iostream/*.cpp -o trogdor-server -pthread -lboost_system -lxml2 -llua$(LUA_VERSION)

client-debug:
	echo "NOT YET IMPLEMENTED"

lcount:
	wc -l */*/*.cpp */*/*/*.cpp */*/*/*/*.cpp */*/*/*/*/*.cpp */*/*/*.h */*/*/*/*.h */*/*/*/*/*.h */*/*/*/*/*/*.h

# No longer current or recently tested (you have been warned)
freebsd:
	$(CXX) -std=c++17 -O2 -I/usr/local/include -I/usr/local/include/lua$(subst .,,$(LUA_VERSION)) -I/usr/local/include/libxml2 -L/usr/local/lib src/core/*.cpp src/core/exception/*.cpp src/core/parser/*.cpp src/core/parser/parsers/*.cpp src/core/instantiator/*.cpp src/core/instantiator/instantiators/*.cpp src/core/lua/*.cpp src/core/lua/api/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp -llua-$(LUA_VERSION) src/standalone/*.cpp src/standalone/actions/*.cpp -o trogdor -pthread -lxml2

# No longer current or recently tested (you have been warned)
freebsd-debug:
	$(CXX) -std=c++17 -I/usr/local/include -I/usr/local/include/lua$(subst .,,$(LUA_VERSION)) -I/usr/local/include/libxml2 -L/usr/local/lib -g src/core/*.cpp src/core/exception/*.cpp src/core/parser/*.cpp src/core/parser/parsers/*.cpp src/core/instantiator/*.cpp src/core/instantiator/instantiators/*.cpp src/core/lua/*.cpp src/core/lua/api/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp -llua-$(LUA_VERSION) src/standalone/*.cpp src/standalone/actions/*.cpp -o trogdor -pthread -lxml2

