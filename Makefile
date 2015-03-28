# User will specify whether to use boost::thread of pthread (boost::thread is the default)
ifeq ($(THREAD), pthread)
	THREAD_MACRO=-DPTHREAD
	THREAD_LFLAGS=-pthread
else
	THREAD_LFLAGS=-lboost_system -lboost_thread
endif

default:
	g++ $(THREAD_MACRO) -O2 -I/usr/include/libxml2 -I/usr/include/lua5.1 src/core/*.cpp src/core/parser/*.cpp src/core/lua/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/standalone/*.cpp -o trogdor $(THREAD_LFLAGS) -lxml2 -llua5.1

server:
	g++ -DPTHREAD -O2 -I/usr/include/libxml2 -I/usr/include/lua5.1 src/core/*.cpp src/core/parser/*.cpp src/core/lua/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/online/server/*.cpp src/online/server/network/*.cpp src/online/server/command/*.cpp src/online/server/command/actions/*.cpp src/online/server/iostream/*.cpp -o trogdor-server -pthread -lboost_system -lboost_thread -lxml2 -llua5.1

client:
	echo "NOT YET IMPLEMENTED"

client-test:
	g++ -DPTHREAD -O2 -I/usr/include/libxml2 -I/usr/include/lua5.1 src/online/client/test.cpp -oclient-test -lboost_system -pthread

debug:
	g++ $(THREAD_MACRO) -I/usr/include/libxml2 -I/usr/include/lua5.1 -g src/core/*.cpp src/core/parser/*.cpp src/core/lua/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/standalone/*.cpp -o trogdor $(THREAD_LFLAGS) -lxml2 -llua5.1

server-debug:
	g++ -DPTHREAD -I/usr/include/libxml2 -I/usr/include/lua5.1 -g src/core/*.cpp src/core/parser/*.cpp src/core/lua/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/online/server/*.cpp src/online/server/network/*.cpp src/online/server/command/*.cpp src/online/server/command/actions/*.cpp src/online/server/iostream/*.cpp -o trogdor-server -pthread -lboost_system -lboost_thread -lxml2 -llua5.1

client-debug:
	echo "NOT YET IMPLEMENTED"

lcount:
	wc -l */*/*.cpp */*/*/*.cpp */*/*/*/*.cpp */*/*/*/*/*.cpp */*/*/*.h */*/*/*/*.h */*/*/*/*/*.h */*/*/*/*/*/*.h

# No longer current or recently tested (you have been warned)
freebsd:
	g++ $(THREAD_MACRO) -O2 -I/usr/local/include -I/usr/local/include/lua51 -I/usr/local/include/libxml2 -L/usr/local/lib src/core/*.cpp src/core/parser/*.cpp src/core/lua/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp -llua-5.1 src/standalone/*.cpp -o trogdor $(THREAD_LFLAGS) -lxml2

# No longer current or recently tested (you have been warned)
freebsd-debug:
	g++ $(THREAD_MACRO) -I/usr/local/include -I/usr/local/include/lua51 -I/usr/local/include/libxml2 -L/usr/local/lib -g src/core/*.cpp src/core/parser/*.cpp src/core/lua/*.cpp src/core/lua/api/entities/*.cpp src/core/iostream/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp -llua-5.1 src/standalone/*.cpp -o trogdor $(THREAD_LFLAGS) -lxml2

