# Connections and Requests

This page details how connections and requests are made.

## Establishing a Connection

Once a client connects to an instance of trogdord, the following JSON response will be returned, signaling to the client that it's ready to receive requests:

```json
{
	"status": "ready"
}
```

Requests should not be sent to the server until this first response is received. Currently, trogdord accepts IPv4 connections only. This will change soon.

## Making a Request

### Anatomy of a Request

For a request to be valid, it must be a proper JSON string and must include at least the following properties:

```json
{
	"method": "...",
	"scope": "...",
	"action": "..."
}
```

Together, the method, scope, and action identify a unique operation. If the scope has defined a default action, then, in that case, the action property may be ommitted. For more details about what these properties mean and how requests are routed, see: [Methods, Scopes, and Actions](./scopes-and-actions.md).

In addition to the three properties listed above, some requests require additional arguments. Arguments should be passed via a fourth property called `args`, which should itself be a valid JSON object with key/value pairs. For example, the following request retrieves the details of the game with id 0 from the server:

```json
{
	"method": "get",
	"scope": "game",
	"args": {
		"id": 0
	}
}
```

### The Server's Response

Once you've established a connection with trogdord and sent off a request, the server will return a JSON response. At a minimum, this response will always contain a property called `status`. This will be a code roughly analagous to an HTTP status code. If the status code is anything other than 200 or 206, a `message` property will also be returned as part of the response, giving you more details about why the request failed.

All the status codes that can currently be returned by trogdord are listed below:

| Code | Meaning |
|-|-|
| 200 | The request was successful |
| 206 | The request was only partially successful, or is only returning a partial response |
| 400 | The request was invalid |
| 404 | The requested action or resource wasn't found |
| 409 | The request failed due to a conflict with an existing resource |
| 500 | An error occurred while trying to execute the request |
| 501 | The requested operation is unsupported by the server's current configuration |

Listed below are a few sample requests and their responses:

#### An Invalid Request

**Request:**

```
I'm not valid JSON!
```

**Response:**

```json
{
	"status": 400,
	"message": "request must be valid JSON"
}
```

A response might also return a status of 400 if the request itself is valid JSON but the supplied arguments are invalid (either required arguments are missing or one or more arguments are in an incorrect format.)

#### A Valid Request that Fails

This example requests a game that doesn't exist.

**Request:**

```json
{
	"method": "get",
	"scope": "game",
	"args": {
		"id": 3
	}
}
```

**Response:**

```json
{
	"status": 404,
	"message": "game not found"
}
```

#### A Valid Response That Returns Data

This example requests a game that does exist.

**Request:**

```json
{
	"method": "get",
	"scope": "game",
	"args": {
		"id": 0
	}
}
```

**Response:**

```json
{
	"status": 200,
	"id": 0,
	"name": "test",
	"definition": "game.xml",
	"current_time": 0,
	"is_running": false
}
```

#### A Valid Response That Doesn't Return Data

This example deletes a game.

**Request:**

```json
{
	"method": "delete",
	"scope": "game",
	"args": {
		"id": 0
	}
}
```

**Response:**

```json
{
	"status": 200
}
```