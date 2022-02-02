# Docker image for trogdord 0.90.0.

FROM ubuntu:20.04

# Pre-configure environment and install build dependencies
ENV DEBIAN_FRONTEND="noninteractive" TZ="UTC"
RUN apt-get update && apt-get -y install sudo bash wget g++ cmake libasio-dev sqlite3 libsqlite3-dev rapidjson-dev libhiredis-dev libxml2 libxml2-dev liblua5.3 liblua5.3-dev libinih1 libinih-dev redis-server

# Build and install the core library + trogdord from source
WORKDIR /usr/local/src
RUN wget https://github.com/crankycyclops/trogdor-pp/archive/refs/tags/0.90.0.tar.gz
RUN tar -zvxpf 0.90.0.tar.gz
WORKDIR /usr/local/src/trogdor-pp-0.90.0/src/core
RUN cmake -DENABLE_SERIALIZE_JSON=ON -DENABLE_SERIALIZE_SQLITE=ON -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=/usr .
RUN make -j$(nproc) trogdor
RUN make install
WORKDIR /usr/local/src/trogdor-pp-0.90.0/src/trogdord
RUN cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_REDIS=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr .
RUN make -j$(nproc) trogdord
RUN make install

# Uninstall compiler and other dev packages since they're no longer needed
RUN apt-get -y remove g++ cmake libasio-dev libsqlite3-dev rapidjson-dev libhiredis-dev libxml2-dev liblua5.3-dev libinih-dev
RUN apt-get -y autoremove

# Create a non-root user to execute trogdord
RUN groupadd -r trogdord && useradd -r -g trogdord -d /home/trogdord -s /sbin/nologin -c "Docker image user" trogdord

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh
USER trogdord

WORKDIR /
ENTRYPOINT ["/entrypoint.sh"]