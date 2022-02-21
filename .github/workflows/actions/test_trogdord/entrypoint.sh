#!/bin/bash

# Install core library as prerequisite
cd src/core
cmake -DENABLE_SERIALIZE_JSON=ON -DENABLE_SERIALIZE_SQLITE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX:PATH=/usr .
make -j2 trogdor
make install

cd ../trogdord
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_REDIS=ON -DCMAKE_INSTALL_PREFIX:PATH=/usr .

# Make sure the entire build for trogdord succeeds, including main.cpp, which
# isn't included in the unit tests
make -j2 trogdord

# Compile and run the unit tests
make -j2 test_trogdord
timeout -s SIGQUIT 10 sudo -u app ./test_trogdord

# This last set of unit tests is testing trogdord's handling of environment
# variable overrides, hence the super long string of variables :)
make -j2 test_trogdord_envvar
TROGDORD_PORT="trogdord_port" TROGDORD_REUSE_ADDRESS="reuse_address" TROGDORD_SEND_TCP_KEEPALIVE="send_keepalive" TROGDORD_LISTEN_IPS="listen_ip" TROGDORD_LOGTO="logto" TROGDORD_INPUT_LISTENERS="input_listen" TROGDORD_OUTPUT_DRIVER="out_drive" TROGDORD_REDIS_HOST="redis_host" TROGDORD_REDIS_USERNAME="redis_username" TROGDORD_REDIS_PASSWORD="redis_password" TROGDORD_REDIS_PORT="redis_port" TROGDORD_REDIS_CONNECTION_TIMEOUT="redis_timeout" TROGDORD_REDIS_CONNECTION_RETRY_INTERVAL="redis_retry" TROGDORD_REDIS_OUTPUT_CHANNEL="redis_output" TROGDORD_REDIS_INPUT_CHANNEL="redis_input" TROGDORD_DEFINITIONS_PATH="definitions_path" TROGDORD_STATE_ENABLED="state_enable" TROGDORD_STATE_AUTORESTORE_ENABLED="autorestore_enable" TROGDORD_STATE_DUMP_SHUTDOWN_ENABLED="state_dump_shutdown" TROGDORD_STATE_CRASH_RECOVERY_ENABLED="state_crash" TROGDORD_STATE_FORMAT="state_format" TROGDORD_STATE_PATH="state_path" TROGDORD_STATE_MAX_DUMPS_PER_GAME="state_max_dumps" TROGDORD_EXTENSIONS_PATH="ext_path" TROGDORD_EXTENSIONS_LOAD="ext_load" timeout -s SIGQUIT 10 sudo -u app ./test_trogdord_envvar