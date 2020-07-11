#!/bin/sh

cd $GITHUB_WORKSPACE/trogdor-pp/trogdor-pp/src/php7/trogdord

# TODO: I haven't implemented unit tests yet, so for now, this is just a test to
# make sure the build isn't broken on any supported PHP version.

# Test build against PHP 7.2
/usr/bin/phpize7.2 && ./configure && make
make clean && /usr/bin/phpize7.2 --clean

# Test build against PHP 7.3
/usr/bin/phpize7.3 && ./configure && make
make clean && /usr/bin/phpize7.3 --clean

# Test build against PHP 7.4
/usr/bin/phpize7.4 && ./configure && make
make clean && /usr/bin/phpize7.4 --clean
