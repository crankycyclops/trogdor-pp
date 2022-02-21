#include <unordered_map>
#include <doctest.h>

#include "../config.h"


// Maps environment variables to what their test values should be set to. This
// relies on outside environment variables to be set to these precise values.
// If this is not the case, the unit tests will fail. These settings aren't
// formatted correctly, but internally they're stored as arbitrary strings, so
// it shouldn't matter.
static std::unordered_map<
    const char *,
    std::pair<const char *, const char *>
> testSettings = {

	{"TROGDORD_PORT",                             {Config::CONFIG_KEY_PORT,                             "trogdord_port"}},
	{"TROGDORD_REUSE_ADDRESS",                    {Config::CONFIG_KEY_REUSE_ADDRESS,                    "reuse_address"}},
	{"TROGDORD_SEND_TCP_KEEPALIVE",               {Config::CONFIG_KEY_SEND_TCP_KEEPALIVE,               "send_keepalive"}},
	{"TROGDORD_LISTEN_IPS",                       {Config::CONFIG_KEY_LISTEN_IPS,                       "listen_ip"}},
	{"TROGDORD_LOGTO",                            {Config::CONFIG_KEY_LOGTO,                            "logto"}},
	{"TROGDORD_INPUT_LISTENERS",                  {Config::CONFIG_KEY_INPUT_LISTENERS,                  "input_listen"}},
	{"TROGDORD_OUTPUT_DRIVER",                    {Config::CONFIG_KEY_OUTPUT_DRIVER,                    "out_drive"}},
	{"TROGDORD_REDIS_HOST",                       {Config::CONFIG_KEY_REDIS_HOST,                       "redis_host"}},
	{"TROGDORD_REDIS_USERNAME",                   {Config::CONFIG_KEY_REDIS_USERNAME,                   "redis_username"}},
	{"TROGDORD_REDIS_PASSWORD",                   {Config::CONFIG_KEY_REDIS_PASSWORD,                   "redis_password"}},
	{"TROGDORD_REDIS_PORT",                       {Config::CONFIG_KEY_REDIS_PORT,                       "redis_port"}},
	{"TROGDORD_REDIS_CONNECTION_TIMEOUT",         {Config::CONFIG_KEY_REDIS_CONNECTION_TIMEOUT,         "redis_timeout"}},
	{"TROGDORD_REDIS_CONNECTION_RETRY_INTERVAL",  {Config::CONFIG_KEY_REDIS_CONNECTION_RETRY_INTERVAL,  "redis_retry"}},
	{"TROGDORD_REDIS_OUTPUT_CHANNEL",             {Config::CONFIG_KEY_REDIS_OUTPUT_CHANNEL,             "redis_output"}},
	{"TROGDORD_REDIS_INPUT_CHANNEL",              {Config::CONFIG_KEY_REDIS_INPUT_CHANNEL,              "redis_input"}},
	{"TROGDORD_DEFINITIONS_PATH",                 {Config::CONFIG_KEY_DEFINITIONS_PATH,                 "definitions_path"}},
	{"TROGDORD_STATE_ENABLED",                    {Config::CONFIG_KEY_STATE_ENABLED,                    "state_enable"}},
	{"TROGDORD_STATE_AUTORESTORE_ENABLED",        {Config::CONFIG_KEY_STATE_AUTORESTORE_ENABLED,        "autorestore_enable"}},
	{"TROGDORD_STATE_DUMP_SHUTDOWN_ENABLED",      {Config::CONFIG_KEY_STATE_DUMP_SHUTDOWN_ENABLED,      "state_dump_shutdown"}},
	{"TROGDORD_STATE_CRASH_RECOVERY_ENABLED",     {Config::CONFIG_KEY_STATE_CRASH_RECOVERY_ENABLED,     "state_crash"}},
	{"TROGDORD_STATE_FORMAT",                     {Config::CONFIG_KEY_STATE_FORMAT,                     "state_format"}},
	{"TROGDORD_STATE_PATH",                       {Config::CONFIG_KEY_STATE_PATH,                       "state_path"}},
	{"TROGDORD_STATE_MAX_DUMPS_PER_GAME",         {Config::CONFIG_KEY_STATE_MAX_DUMPS_PER_GAME,         "state_max_dumps"}},
	{"TROGDORD_EXTENSIONS_PATH",                  {Config::CONFIG_KEY_EXTENSIONS_PATH,                  "ext_path"}},
	{"TROGDORD_EXTENSIONS_LOAD",                  {Config::CONFIG_KEY_EXTENSIONS_LOAD,                  "ext_load"}}
};


TEST_SUITE("Config (config.cpp)") {

    TEST_CASE("Config (config.cpp): Environment variables override default values") {

        Config::get()->load("");

        for (auto const &setting: testSettings) {

            if (const char *value = std::getenv(setting.first); value) {

                std::string configVal = Config::get()->getString(setting.second.first);

                if (0 != configVal.compare(setting.second.second)) {
                    std::string failMsg = std::string("Expected value of \"") + setting.second.second +
                        "\" for " + setting.second.first + " but got \"" + configVal + "\" instead.";
                    FAIL(failMsg);
                }
            }

            else {
                std::string failMsg = std::string("Environment variable ") + setting.first + " must be set for the unit test result to be valid.";
                FAIL(failMsg);
            }
        }
    }

    TEST_CASE("Config (config.cpp): Environment variables override trogdord.ini") {

        std::unordered_map<std::string, std::string> initialValues;
        std::string iniFilename = STD_FILESYSTEM::temp_directory_path().string() + "/test.ini";

        // I'll set every ini value to "blah" and make sure they all get overridden
        for (auto const &setting: testSettings) {

            initialValues[setting.second.first] = "blah";

            // Make sure necessary environment variables are set!
            if (const char *value = std::getenv(setting.first); !value) {
                std::string failMsg = std::string("Environment variable ") + setting.first + " must be set for the unit test result to be valid.";
                FAIL(failMsg);
            }
        }

        initIniFile(iniFilename, initialValues);

        // Next, setup and load the ini file, then verify that the environment variables
        // took precedence
        for (auto const &setting: testSettings) {
            CHECK(0 == Config::get()->getString(setting.second.first).compare(setting.second.second));
        }

        // Cleanup
        STD_FILESYSTEM::remove(iniFilename);
    }
}
