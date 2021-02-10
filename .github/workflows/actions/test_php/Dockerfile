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
RUN apt-get -y install git g++ zlib1g-dev libonig-dev libzip5 libzip-dev

# Build and install each PHP version from source
COPY buildphp.sh /buildphp.sh
RUN chmod +x /buildphp.sh
RUN /buildphp.sh 7.2.34
RUN /buildphp.sh 7.3.27
RUN /buildphp.sh 7.4.15
RUN /buildphp.sh 8.0.2

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
