# Methods, Scopes, and Actions

Every request that trogdord receives can be broken down into a method, a scope, and an action. These three values together map to a specific operation that trogdord should perform.

## Methods

The method component of a request is analagous to the HTTP methods GET, PUT, POST, DELETE, etc. and determines what sort of operation you wish to perform. The following methods have been defined and are understood by trogdord:

| Method | Meaning |
|-|-|
| get | These requests retrieve resources and data from the server and are analagous to the HTTP verb GET. |
| post | These requests create new resources and data and are analagous to the HTTP verb POST. |
| put | These requests replace already existing resources and data. |
| set | These requests are like HTTP's PATCH and modify (but don't replace) existing resources and data. |
| delete | These requests are like HTTP'S DELETE and destroy resources and data. |

## Scopes

If trogdord were an MVC application, then the scope would be your controller. Scopes are organized according to how general or specific the resource is that you're querying, as well as the type. The following scopes have been defined by trogdord:

| Scope | Meaning |
|-|-|
| global | Requests that have an effect on the server as a whole. |
| game | Requests related to games. |
| entity | Requests related to entities of any type. |
| resource | Requests related to entities of type Resource. |
| tangible | Requests related to entities of type Tangible. |
| place | Requests related to entities of type Place. |
| room | Requests related to entities of type Room. |
| thing | Requests related to entities of type Thing. |
| object | Requests related to entities of type Object. |
| being | Requests related to entities of type Being. |
| creature | Requests related to entities of type Creature. |
| player | Requests related to players. |

## Actions

It is the action component of a request that, combined with the method and scope, identifies the specific operation you wish to perform. For example, a request with the method "get", the scope "game", and the action "list" will return a list of all games that currently exist on the server.

Sometimes, a scope will define a default action. In that case, the action may be left out and trogdord will still know how to route your request. For example, specifying the method "get" with the scope "game" with no action will retrieve the details of a specific game from the server.

## A Complete Reference

Below, you'll find a complete reference of all currently defined actions as well as how to format your requests.

### Global Scope

#### 1. get:global:config

Retrieves non-sensitive settings from trogdord.ini.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "global",
	"action": "config"
}
```

**Arguments:**

(none)

**Successful JSON Response:**

Returns a status code and all non-sensitive configuration settings.

```
{
	"status": 200,
	"config": {
		"<setting>": <int|boolean|string>,
		...
	}
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |

---

#### 2. get:global:statistics

Retrieves statistical information about the server.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "global",
	"action": "statistics"
}
```

**Arguments:**

(none)

**Successful JSON Response:**

Returns a status code, the number of players, and the version of both the core library and trogdord.

```
{
	"status": 200,
	"players": <unsigned int>,
	"version": {
		"major": <unsigned int>,
		"minor": <unsigned int>,
		"patch": <unsigned int>
	},
	"lib_version": {
		"major": <unsigned int>,
		"minor": <unsigned int>,
		"patch": <unsigned int>
	}
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |

---

#### 3. post:global:dump

Dumps the server's current state (including all games) to disk. This state can
later be restored.

**JSON Request Format:**

```
{
	"method": "post",
	"scope": "global",
	"action": "dump"
}
```

**Arguments:**

(none)

**Successful JSON Response:**

Returns a status code.

```
{
	"status": 200
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 500 | An error occurred |
| 501 | State feature was disabled in trogdord.ini |

---

#### 4. post:global:restore

Restores the server's current state (including all games) from disk.

**JSON Request Format:**

```
{
	"method": "post",
	"scope": "global",
	"action": "restore"
}
```

**Arguments:**

(none)

**Successful JSON Response:**

Returns a status code.

```
{
	"status": 200
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 206 | Partial success (only some games could be restored, not all) |
| 500 | An error occurred |
| 501 | State feature was disabled in trogdord.ini |

---

### Game Scope

#### 1. get:game

Returns the details of an existing game, referenced by id.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"args": {
		"id": <unsigned integer>,
		"name": <string>,
		"definition": <string>,
		"created": <unsigned int>,
		"current_time": <int>,
		"is_running": <boolean>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | The id of the game whose details you wish to retrieve |

**Successful JSON Response:**

Returns a status code, the game's id, the game's name, the definition filename that was used to create the game, the time the game was created (UNIX timestamp), the current time in the game, and whether or not the game is running.

```
{
	"status": 200,
	"id": <unsigned int>,
	"name": <string>,
	"definition": <string>,
	"created": <unsigned int>,
	"current_time": <unsigned int>,
	"is_running": <boolean>
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Invalid Game Id |
| 404 | Game not found |

---

#### 2. post:game

Creates a new game and returns its id as part of a successful response.

**JSON Request Format:**

```
{
	"method": "post",
	"scope": "game",
	"args": {
		"id": <int>,
		"created": <unsigned int>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `name` | Yes | The game's name |
| `definition` | Yes | The definition file that should be used to create the game (must be one of the files returned by get:game:definitions) |
| One or more key/value pairs | No | Meta data that should be set for the game when it's created |

**Successful JSON Response:**

Returns a status code, the new game's id, and the time the game was created (UNIX timestamp.)

```
{
	"status": 200,
	"id": <unsigned int>,
	"name": <string>,
	"definition": <string>,
	"created": <unsigned int>
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Missing one or more required arguments, or the definition path or one or more meta value arguments are invalid |
| 500 | An internal error occurred |

---

#### 3. delete:game

Destroys an existing game.

**JSON Request Format:**

```
{
	"method": "delete",
	"scope": "game",
	"args": {
		"id": <unsigned int>,
		"delete_dump": <boolean> (optional)
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | The game's id |
| `delete_dump` | No | Whether or not to also delete all dumps of the game (true by default) |

**Successful JSON Response:**

Returns a status code indicating success.

```
{
	"status": 200
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 404 | Game not found |

---

#### 4. delete:game:dump

Deletes a game's entire dump history or just a specific dump slot, depending on
the arguments passed.

**JSON Request Format:**

```
{
	"method": "delete",
	"scope": "game",
	"action": "dump",
	"args": {
		"id": <unsigned int>,
		"slot": <unsigned int> (optional)
	}
}
```

Passing just an id results in the game's entire dump history being deleted.
Passing a slot results in just that dump slot being deleted.

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | The game's id |
| `slot` | No | Dump slot to delete. If this argument isn't passed, the game's entire dump history will be destroyed. |

**Successful JSON Response:**

Returns a status code indicating success.

```
{
	"status": 200
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Request was invalid |
| 404 | Game or slot not found |
| 500 | An internal server error occurred |
| 501 | State feature was disabled in trogdord.ini |

---

#### 5. get:game:list

Returns a list of all games that currently exist on the server.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"action": "list",
	"args": {
		"filters": {...} || [{...}, ...]
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `filters` | No | A filter group or filter union in JSON format. For more information about what filters are and how they work see: [Filters](./filters.md). |

**Filters:**

The following filters are accepted by this request and determine which games are listed in the response:

| Filter | Valid Values | Matches |
|-|-|-|
| `is_running` | boolean | Games that are either running (true) or not running (false) |
| `name_starts` | string | Games whose names are prefixed by that value |

**Successful JSON Response:**

Returns a status code and an array of key/value pairs containing the id and name of every existing game. If filters were applied,only games matching their criteria will be listed.

```
{
	"status": 200,
	"games": [
		{
			"id": <unsigned int>,
			"name": <string>,
			"definition": <string>,
			"created": <unsigned int>
		},
		...
	]
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | One or more filters were invalid |

---

#### 6. get:game:meta

Returns metadata for a specific game. Can either return specific values or all values depending on the arguments passed along with the request.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"action": "meta",
	"args": {
		"id": <unsigned int>,
		"meta": [<string>, ...]
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of the game we're retrieving the metadata from. |
| `meta` | No | Array of string keys representing the meta values that should be returned in the response. If any of those meta values don't exist, an empty string will be returned. |

**Successful JSON Response:**

Returns a status code and an object of key/value pairs representing the requested game-specific metadata.

```
{
	"status": 200,
	"meta": {
		<string key>: <string value>
	}
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Invalid Game Id |
| 404 | Game not found |

---

#### 7. get:game:statistics

Returns game-specific statistics.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"action": "statistics",
	"args": {
		"id": <unsigned int>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |

**Successful JSON Response:**

Returns a status code, the time the game was created, how many players are currently in the game, the current game time, and whether or not the game is running.

```
{
	"status": 200,
	"created": <unsigned int>,
	"players": <unsigned int>,
	"current_time": <unsigned int>,
	"is_running": <boolean>
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Invalid Game Id |
| 404 | Game not found |

---

#### 8. get:game:time

Returns the current time in a game.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"action": "time",
	"args": {
		"id": <unsigned int>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |

**Successful JSON Response:**

Returns a status code and the current in-game time.

```
{
	"status": 200,
	"current_time": <unsigned int>
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 404 | Game not found |

---

#### 9. get:game:is_running

Returns true if the specified game is running and false if it's not.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"action": "is_running",
	"args": {
		"id": <unsigned int>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |

**Successful JSON Response:**

Returns a status code and whether or not the game is running.

```
{
	"status": 200,
	"is_running": <boolean>
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 404 | Game not found |

---

#### 10. get:game:definitions

Returns a list of all game definition files seen by the server. These are the files that can be used to make new games.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"action": "definitions"
}
```

**Arguments:**

(none)

**Successful JSON Response:**

Returns a status code and a JSON array of all available definition files.

```
{
	"status": 200,
	"definitions": [
		<string>,
		...
	]
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 500 | An internal error occurred |

---

#### 11. get:game:dump

TODO

---

#### 12. get:game:dumplist

TODO

---

#### 13. set:game:start

Starts the specified game.

**JSON Request Format:**

```
{
	"method": "set",
	"scope": "game",
	"action": "start",
	"args": {
		"id": <unsigned int>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |

**Successful JSON Response:**

Returns a status code indicating success.

```
{
	"status": 200
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 404 | Game not found |

---

#### 14. set:game:stop

Stops the specified game.

**JSON Request Format:**

```
{
	"method": "set",
	"scope": "game",
	"action": "stop",
	"args": {
		"id": <unsigned int>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |

**Successful JSON Response:**

Returns a status code indicating success.

```
{
	"status": 200
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 404 | Game not found |

---

#### 15. set:game:meta

Sets meta data associated with the specified game.

**JSON Request Format:**

```
{
	"method": "set",
	"scope": "game",
	"action": "meta",
	"args": {
		"id": <unsigned int>,
		"meta": <object of key/value pairs>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |
| `meta` | Yes | Object containing key/value pairs of data |

**Successful JSON Response:**

Returns a status code indicating success.

```
{
	"status": 200
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Request was invalid |
| 404 | Game not found |

---

#### 16. post:game:dump

Dumps the specified game.

**JSON Request Format:**

```
{
	"method": "post",
	"scope": "game",
	"action": "dump",
	"args": {
		"id": <unsigned int>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |

**Successful JSON Response:**

Returns a status code indicating success and the dump slot (like a version) the
game was saved to.

```
{
	"status": 200,
	"slot": 0
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Invalid request |
| 404 | Game not found |
| 500 | An internal error occured |
| 501 | State feature was disabled in trogdord.ini |

---

#### 17. post:game:restore

Restores the specified dumped game.

**JSON Request Format:**

```
{
	"method": "post",
	"scope": "game",
	"action": "restore",
	"args": {
		"id": <unsigned int>,
		"slot": <unsigned int> (optional)
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | Id of an existing game |
| `slot` | No | The dump slot (like a version) to restore (defaults to the latest slot) |

**Successful JSON Response:**

Returns a status code indicating success and the dump slot (like a version) that
was restored.

```
{
	"status": 200,
	"slot": 0
}
```

**Possible Status Codes:**

| Status | Meaning |
|-|-|
| 200 | Success |
| 400 | Invalid request |
| 404 | Game or slot not found |
| 500 | An internal error occured |
| 501 | State feature was disabled in trogdord.ini |

---

TODO: finish documenting the remaining entity scopes