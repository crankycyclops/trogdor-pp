default:
	g++ -I/usr/include/libxml2 -I/usr/include/lua5.1 src/core/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/standalone/*.cpp -o trogdor -pthread -lxml2 -llua5.1

debug:
	g++ -I/usr/include/libxml2 -I/usr/include/lua5.1 -g src/core/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp src/standalone/*.cpp -o trogdor -pthread -lxml2 -llua5.1

freebsd:
	g++ -I/usr/local/include -I/usr/local/include/lua51 -I/usr/local/include/libxml2 -L/usr/local/lib -g src/core/*.cpp src/core/event/*.cpp src/core/event/triggers/*.cpp src/core/timer/jobs/*.cpp src/core/entities/*.cpp -llua-5.1 src/standalone/*.cpp -o trogdor -pthread -lxml2

lcount:
	wc -l */*/*.cpp */*/*/*.cpp */*/*/*/*.cpp */*/*/*.h */*/*/*/*.h */*/*/*/*/*.h

