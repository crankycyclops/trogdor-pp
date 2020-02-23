#include "include/request.h"


// String representations of the five request methods
const char *Request::GET = "get";
const char *Request::POST = "post";
const char *Request::PUT = "put";
const char *Request::SET = "set";
const char *Request::DELETE = "delete";

// Error messages for missing or invalid arguments
const char *Request::MISSING_GAME_ID = "missing required game id";
const char *Request::INVALID_GAME_ID = "invalid game id";
