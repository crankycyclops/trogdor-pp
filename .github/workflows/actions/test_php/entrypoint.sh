#!/bin/sh

cd src/php7/trogdord

# TODO: I haven't implemented unit tests yet, so for now, this is just a test to
# make sure the build isn't broken on any supported PHP version.

# Test build against PHP 7.2
/usr/bin/phpize7.2 && ./configure && make

if [ $? -ne 0 ];
	exit 1
fi

make clean && /usr/bin/phpize7.2 --clean

if [ $? -ne 0 ];
	exit 1
fi

# Test build against PHP 7.3
/usr/bin/phpize7.3 && ./configure && make

if [ $? -ne 0 ];
	exit 1
fi

make clean && /usr/bin/phpize7.3 --clean

if [ $? -ne 0 ];
	exit 1
fi

# Test build against PHP 7.4
/usr/bin/phpize7.4 && ./configure && make

if [ $? -ne 0 ];
	exit 1
fi

make clean && /usr/bin/phpize7.4 --clean

if [ $? -ne 0 ];
	exit 1
fi
