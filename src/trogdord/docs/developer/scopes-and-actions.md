# Methods, Scopes, and Actions

Every request that trogdord receives can be broken down into a method, a scope, and an action. These three values together map to a specific operation that trogdord should perform.

## Methods

The method component of a request is analagous to the HTTP methods GET, PUT, POST, DELETE, etc. and determines what sort of operation you wish to perform. The following methods have been defined and are understood by trogdord:

| Method | Meaning |
|-|-|
| **get** | These requests retrieve resources and data from the server and are analagous to the HTTP verb GET. |
| **post** | These requests create new resources and data and are analagous to the HTTP verb POST. |
| **put** | These requests replace already existing resources and data. |
| **set** | These requests are like HTTP's PATCH and modify (but don't replace) existing resources and data. |
| **delete** | These requests are like HTTP'S DELETE and destroy resources and data. |

## Scopes

If trogdord were an MVC application, then the scope would be your controller. Scopes are organized according to how general or specific the resource is that you're querying, as well as the type. The following scopes have been defined by trogdord:

| Scope | Meaning |
|-|-|
| **global** | Requests that have an effect on the server as a whole. |
| **game** | Requests related to games. |
| **entity** | Requests related to entities of any type. |
| **place** | Requests related to entities of type Place. |
| **room** | Requests related to entities of type Room. |
| **thing** | Requests related to entities of type Thing. |
| **object** | Requests related to entities of type Object. |
| **being** | Requests related to entities of type Being. |
| **creature** | Requests related to entities of type Creature. |
| **player** | Requests related to players. |

## Actions

It is the action component of a request that, combined with the method and scope, identifies the specific operation you wish to perform. For example, a request with the method "get", the scope "game" and the action "list" will return a list of all games that currently exist on the server.

Sometimes, a scope will define a default action. In that case, the action may be left out and trogdord will still know how to route your request. For example, specifying the method "get" with the scope "game" with no action will retrieve the details of a specific game from the server.

## A Complete Reference

Below, you'll find a complete reference of all currently defined actions as well as how to format your requests.

### Global Scope

#### get:global:statistics

Retrieves statistical information about the server, including the version of trogdord, the version of the core library, and the total number of existing players and games.

Request Format:

```json
{
	"method": "get",
	"scope": "global",
	"action": "statistics"
}
```

### Game Scope

#### get:game

Returns a specific game (if it exists), referenced by its integer ID.

Request Format:

```json
{
	"method": "get",
	"scope": "game",
	"args": {
		"id": <unsigned int>
	}
}
```

#### get:game:list

Returns a list of all games that currently exist on the server.

#### get:game:meta

Returns metadata for a specific game.

#### get:game:statistics

Returns game-specific statistics.

#### get:game:time

Returns the current time in a game.

#### get:game:is_running

Returns true if the specified game is running and false if it's not.

#### get:game:definitions

Returns a list of all game definition files seen by the server. These are the files that can be used to make new games.

TODO: finish filling in the reference