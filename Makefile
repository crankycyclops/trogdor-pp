default:
	g++ -g -I/usr/include/libxml2 -I/usr/include/lua5.1 src/core/*.cpp src/standalone/*.cpp -lxml2 -llua5.1 -o trogdor
