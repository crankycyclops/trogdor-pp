#include "include/response.h"


// If I don't include these, the linker will throw errors. The actual values are
// defined in response.h.
const int Response::STATUS_SUCCESS;
const int Response::STATUS_INVALID;
const int Response::STATUS_NOT_FOUND;
const int Response::STATUS_CONFLICT;
const int Response::STATUS_INTERNAL_ERROR;
const int Response::STATUS_UNSUPPORTED;

// Default generic message that should be returned when an internal error occurs
const char *Response::INTERNAL_ERROR_MSG = "An internal error occurred";
