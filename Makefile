default:
	g++ src/core/*.cpp src/standalone/*.cpp -o trogdor -pthread

debug:
	g++ -g src/core/*.cpp src/standalone/*.cpp -o trogdor -pthread

freebsd:
	g++ -I/usr/local/include -g src/core/*.cpp src/standalone/*.cpp -o trogdor -pthread

old:
	g++ -g -I/usr/include/libxml2 -I/usr/include/lua5.1 src/core/*.cpp src/standalone/*.cpp -lxml2 -llua5.1 -o trogdor
