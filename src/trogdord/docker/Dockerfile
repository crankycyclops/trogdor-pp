# Docker image for trogdord.
# To build: docker build --build-arg TROGDOR_VER=<version> -t crankycyclops/trogdord:<version> .

FROM ubuntu:20.04

ARG TROGDOR_VER
ENV TROGDOR_VER=${TROGDOR_VER}

# Pre-configure environment and install build dependencies
ENV DEBIAN_FRONTEND="noninteractive" TZ="UTC"
RUN apt-get update
RUN apt-get dist-upgrade -y
RUN apt-get -y install sudo bash wget g++ cmake libasio-dev sqlite3 libsqlite3-dev rapidjson-dev libhiredis0.14 libhiredis-dev libxml2 libxml2-dev liblua5.3 liblua5.3-dev libinih1 libinih-dev

# Build and install the core library + trogdord from source
WORKDIR /usr/local/src
RUN wget https://github.com/crankycyclops/trogdor-pp/archive/refs/tags/$TROGDOR_VER.tar.gz
RUN tar -zvxpf $TROGDOR_VER.tar.gz
WORKDIR /usr/local/src/trogdor-pp-$TROGDOR_VER/src/core
RUN cmake -DENABLE_SERIALIZE_JSON=ON -DENABLE_SERIALIZE_SQLITE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/usr .
RUN make -j$(nproc) trogdor
RUN make install
WORKDIR /usr/local/src/trogdor-pp-$TROGDOR_VER/src/trogdord
RUN cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_REDIS=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr .
RUN make -j$(nproc) trogdord
RUN make install

# Uninstall compiler and other dev packages since they're no longer needed
RUN apt-get -y remove g++ cmake libasio-dev libsqlite3-dev rapidjson-dev libhiredis-dev libxml2-dev liblua5.3-dev libinih-dev
RUN apt-get -y autoremove

# Create a non-root user to execute trogdord
RUN groupadd -r trogdord -g 999 && useradd -u 999 -r -g trogdord -d /home/trogdord -s /sbin/nologin -c "trogdord" trogdord

# Make sure volumes that get mounted here by docker-compose will be owned by trogdord
RUN mkdir -p /usr/var/trogdord && chown trogdord:trogdord /usr/var/trogdord && chmod u+s /usr/var/trogdord && chmod g+s /usr/var/trogdord
RUN mkdir -p /usr/var/trogdord/state && chown trogdord:trogdord /usr/var/trogdord/state && chmod u+s /usr/var/trogdord/state && chmod g+s /usr/var/trogdord/state

WORKDIR /

USER trogdord
ENTRYPOINT ["/usr/bin/trogdord"]
