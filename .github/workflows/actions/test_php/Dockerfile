FROM ubuntu:20.04

# Add PHP repository
RUN apt-get update && apt-get install -y software-properties-common
RUN LC_ALL=C.UTF-8 add-apt-repository -y ppa:ondrej/php && apt-get update

# Prevents the script from stalling due to requiring the user to pick a timezone
RUN export DEBIAN_FRONTEND=noninteractive DEBCONF_NONINTERACTIVE_SEEN=true
RUN apt-get install -y tzdata

# Install dependencies for trogdord so we can build and use it for our PHP tests
RUN apt-get -y install sudo bash wget git g++ cmake libasio-dev rapidjson-dev libhiredis-dev libxml2 libxml2-dev liblua5.3 liblua5.3-dev libinih1 libinih-dev redis-server

# Install build dependencies for each supported PHP version
RUN apt-get update
RUN apt-get -y install git g++ libasio-dev rapidjson-dev php7.2-cli php7.2-dev php7.3-cli php7.3-dev php7.4-cli php7.4-dev

# Build and install PHP 8.0 from source
RUN /buildphp.sh 8.0.2

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
