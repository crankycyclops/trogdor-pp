#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/usr/local/lib"
export TEST_PHP_ARGS="-q"

###############################################################################

runTest() {

	PHPVER=$1

	export PATH=/usr/local/php$PHPVER/bin:$PATH
	phpize && ./configure && make

	if [ $? -ne 0 ]; then
		exit 1
	fi

	##########################
	# Part 1: State disabled #
	##########################

	printf "[state]\nenabled=false\n" >> /usr/local/etc/trogdord/trogdord.ini

	trogdord &
	TROGDORD_PID=$!

	make test

	if [ $? -ne 0 ]; then
		return 1
	fi

	kill $TROGDORD_PID
	sleep 1

	#########################
	# Part 2: State enabled #
	#########################

	printf "[state]\nenabled=true\nmax_dumps_per_game=5\nsave_path=var/trogdord/state\n" >> /usr/local/etc/trogdord/trogdord.ini

	trogdord &
	TROGDORD_PID=$!

	make test

	if [ $? -ne 0 ]; then
		return 1
	fi

	kill $TROGDORD_PID
	sleep 1

	make clean && phpize --clean

	if [ $? -ne 0 ]; then
		return 1
	fi
}

###############################################################################

##############################
#       Build trogdord       #
##############################

cd src/core
cmake -DENABLE_SERIALIZE_JSON=ON -DENABLE_SERIALIZE_SQLITE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=/usr .
make -j2 trogdor
make install

cd ../trogdord
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr .
make -j2 trogdord
make install

cd ../php7/trogdord

##############################
#          Run tests         #
##############################

runTest 7.2

if [ $? -ne 0 ]; then
	exit 1
fi

runTest 7.3

if [ $? -ne 0 ]; then
	exit 1
fi

runTest 7.4

if [ $? -ne 0 ]; then
	exit 1
fi

runTest 8.0

if [ $? -ne 0 ]; then
	exit 1
fi
