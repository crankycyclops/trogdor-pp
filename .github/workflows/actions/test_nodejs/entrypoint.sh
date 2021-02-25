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

cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON -DENABLE_SERIALIZE_JSON=ON .

if [ $? -ne 0 ]; then
	exit 1
fi

make -j2 trogdord

if [ $? -ne 0 ]; then
	exit 1
fi

make install

if [ $? -ne 0 ]; then
	exit 1
fi

cd src/nodejs/trogdord

##############################
#          Run tests         #
##############################

runTest

if [ $? -ne 0 ]; then
	exit 1
fi
