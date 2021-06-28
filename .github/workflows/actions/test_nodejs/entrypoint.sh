#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/usr/local/lib"
export TEST_PHP_ARGS="-q"

###############################################################################

runTest() {

	##########################
	# Part 1: State disabled #
	##########################

	printf "[state]\nenabled=false\n" >> /usr/local/etc/trogdord/trogdord.ini

	trogdord &
	TROGDORD_PID=$!

	npm test

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

	npm test

	if [ $? -ne 0 ]; then
		return 1
	fi

	kill $TROGDORD_PID
	sleep 1
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

cd ../nodejs/trogdord

##############################
#          Run tests         #
##############################

runTest

if [ $? -ne 0 ]; then
	exit 1
fi
