#!/bin/sh

export TEST_PHP_ARGS="-q"

cd src/php7/trogdord

trogdord &
$TROGDORD_PID=$!

# Test build against PHP 7.2
/usr/bin/phpize7.2 && ./configure && make

if [ $? -ne 0 ]; then
	exit 1
fi

make test

if [ $? -ne 0 ]; then
	exit 1
fi

make clean && /usr/bin/phpize7.2 --clean

if [ $? -ne 0 ]; then
	exit 1
fi

kill $TROGDORD_PID

trogdord &
$TROGDORD_PID=$!

# Test build against PHP 7.3
/usr/bin/phpize7.3 && ./configure && make

if [ $? -ne 0 ]; then
	exit 1
fi

make test

if [ $? -ne 0 ]; then
	exit 1
fi

make clean && /usr/bin/phpize7.3 --clean

if [ $? -ne 0 ]; then
	exit 1
fi

kill $TROGDORD_PID

trogdord &
$TROGDORD_PID=$!

# Test build against PHP 7.4
/usr/bin/phpize7.4 && ./configure && make

if [ $? -ne 0 ]; then
	exit 1
fi

make test

if [ $? -ne 0 ]; then
	exit 1
fi

make clean && /usr/bin/phpize7.4 --clean

if [ $? -ne 0 ]; then
	exit 1
fi

kill $TROGDORD_PID

#trogdord &
#$TROGDORD_PID=$!

# Test build against PHP 8.0
#/usr/bin/phpize8.0 && ./configure && make

#if [ $? -ne 0 ]; then
#	exit 1
#fi

#make test

#if [ $? -ne 0 ]; then
#	exit 1
#fi

#make clean && /usr/bin/phpize8.0 --clean

#if [ $? -ne 0 ]; then
#	exit 1
#fi

#kill $TROGDORD_PID