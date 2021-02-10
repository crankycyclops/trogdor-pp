#!/bin/sh

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:"/usr/local/lib"
export TEST_PHP_ARGS="-q"

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

cd src/php7/trogdord

trogdord &
TROGDORD_PID=$!

# Test build against PHP 7.2
/usr/local/php7.2/bin/phpize && ./configure && make

if [ $? -ne 0 ]; then
	exit 1
fi

make test

if [ $? -ne 0 ]; then
	exit 1
fi

make clean && /usr/local/php7.2/bin/phpize --clean

if [ $? -ne 0 ]; then
	exit 1
fi

kill $TROGDORD_PID
sleep 1

trogdord &
TROGDORD_PID=$!

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
sleep 1

trogdord &
TROGDORD_PID=$!

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
sleep 1

trogdord &
TROGDORD_PID=$!

# Test build against PHP 8.0
export PATH=/usr/local/php8.0/bin:$PREFIX/composer/$VER/vendor/bin:$PATH
phpize && ./configure && make

if [ $? -ne 0 ]; then
	exit 1
fi

make test

if [ $? -ne 0 ]; then
	exit 1
fi

make clean && phpize --clean

if [ $? -ne 0 ]; then
	exit 1
fi

kill $TROGDORD_PID
sleep 1
