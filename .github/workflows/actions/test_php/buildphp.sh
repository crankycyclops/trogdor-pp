#!/bin/bash

VER=$1
PREFIX=$2

DEFAULT_PREFIX="/usr/local"
DOWNLOAD_URL="https://www.php.net/distributions"
CONFIGURE_DEBUG_FLAGS="--enable-debug --enable-phpdbg-debug"
CONFIGURE_FLAGS="--enable-zip --enable-bcmath --enable-ctype --enable-json --enable-mbstring --enable-pdo --with-pdo-mysql --enable-tokenizer --enable-xml --enable-fpm --with-openssl --without-sqlite3 --without-pdo-sqlite --disable-opcache"

if [ -z $VER ]; then

	echo
	echo "$0 <full php version (ex: 7.3.13)>"
	echo

	exit 1
fi

if [ -z $PREFIX ]; then
    PREFIX=$DEFAULT_PREFIX
fi

if [ ! -d $PREFIX/php$VER ]; then

	# If the source tree doesn't already exist, download and extract it
	if [ ! -d $PREFIX/src/php-$VER ]; then

		echo
		echo "Downloading php-$VER.tar.bz2..."
		echo

		cd /tmp && wget -c $DOWNLOAD_URL/php-$VER.tar.bz2

		if [ 0 -ne $? ]; then

			echo
			echo "php-$VER.tar.bz2 does not exist or could not be downloaded."
			echo

			exit 2
		fi

		echo
		echo "Extracting php-$VER.tar.bz2..."
		echo

		cd $PREFIX/src && tar -jxf /tmp/php-$VER.tar.bz2 > /dev/null

		if [ 0 -ne $? ]; then

			rm -rf $PREFIX/src/php-$VER

			echo
			echo "Extraction failed."
			echo

			exit 3
		fi
	fi

	echo
	echo "Building php-$VER..."
	echo

	cd $PREFIX/src/php-$VER

	./configure $CONFIGURE_DEBUG_FLAGS $CONFIGURE_FLAGS -with-fpm-user=james --with-fpm-group=james --prefix=$PREFIX/php$VER

	if [ 0 -ne $? ]; then

		echo
		echo "Build failed."
		echo

		exit 4
	fi

	make

	if [ 0 -ne $? ]; then

		echo
		echo "Build failed."
		echo

		exit 4
	fi

	echo
	echo "Installing php-$VER..."
	echo

	make install

	if [ 0 -ne $? ]; then

		echo
		echo "Build failed."
		echo

		exit 4
	fi

	echo
	echo "Configuring php-$VER..."
	echo

	cp $PREFIX/src/php-$VER/php.ini-development $PREFIX/php$VER/lib/php.ini
	cp $PREFIX/php$VER/etc/php-fpm.conf.default $PREFIX/php$VER/etc/php-fpm.conf
	cp $PREFIX/php$VER/etc/php-fpm.d/www.conf.default $PREFIX/php$VER/etc/php-fpm.d/www.conf
	sed -i 's/;pid = run\/php-fpm.pid/pid = run\/php-fpm.pid/g' $PREFIX/php$VER/etc/php-fpm.conf
	sed -i 's/;date.timezone =/date.timezone = "America\/Los_Angeles"/g' $PREFIX/php$VER/lib/php.ini
	sed -i 's/;cgi.fix_pathinfo=1/cgi.fix_pathinfo=0/g' $PREFIX/php$VER/lib/php.ini

	echo >> $PREFIX/php$VER/lib/php.ini
	echo "; Load the trogdord extension" >> $PREFIX/php$VER/lib/php.ini
	echo "extension=trogdord.so" >> $PREFIX/php$VER/lib/php.ini

	echo
	echo "Finished! Enjoy :)"
	echo

else

	echo
	echo "PHP $VER is already installed ;)"
	echo

fi
