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

#### 1. get:global:statistics

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

### Game Scope

#### 1. get:game

Returns the details of an existing game, referenced by id.

**JSON Request Format:**

```
{
	"method": "get",
	"scope": "game",
	"args": {
		"id": <unsigned integer>
	}
}
```

**Arguments:**

| Argument | Required? | Value |
|-|-|-|
| `id` | Yes | The id of the game whose details you wish to retrieve |

**Successful JSON Response:**

Returns a status code, the game's id, the game's name, the definition filename that was used to create the game, the current time in the game, and whether or not the game is running.

```
{
	"status": 200,
	"id": <unsigned int>,
	"name": <string>,
	"definition": <string>,
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

#### 2. get:game:list

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
		{"id": <unsigned int>, "name": <string>},
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

#### 3. get:game:meta

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

#### 4. get:game:statistics

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
	"created": <timestamp>,
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

#### 5. get:game:time

Returns the current time in a game.

TODO

---

#### 6. get:game:is_running

Returns true if the specified game is running and false if it's not.

TODO

---

#### 7. get:game:definitions

Returns a list of all game definition files seen by the server. These are the files that can be used to make new games.

TODO

---

TODO: finish