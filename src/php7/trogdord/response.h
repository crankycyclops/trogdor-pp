#ifndef PHP_RESPONSE_H
#define PHP_RESPONSE_H


// All possible status codes that can be returned by a response
static constexpr unsigned STATUS_SUCCESS = 200;
static constexpr unsigned STATUS_PARTIAL_CONTENT = 206;
static constexpr unsigned STATUS_INVALID = 400;
static constexpr unsigned STATUS_NOT_FOUND = 404;
static constexpr unsigned STATUS_CONFLICT = 409;
static constexpr unsigned STATUS_INTERNAL_ERROR = 500;
static constexpr unsigned STATUS_UNSUPPORTED = 501;


#endif