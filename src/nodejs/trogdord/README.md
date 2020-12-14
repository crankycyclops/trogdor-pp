# Trogdord Node.js Module

The official Node.js client for trogdord.

## Dependencies

* [Node.js](https://nodejs.org/) 12+

## Installaton

```
npm install trogdord
```

## How to use

### Establishing a Connection

By default, the Trogdord object will attempt to connect to localhost:1040 (1040 is the default port that trogdord runs on.) If those values are correct, then all you need to do is this:

```javascript
const connection = new Trogdord();
```

To specify a different hostname, pass it into the first argument of the constructor:

```javascript
const connection = new Trogdord('myhostname.com');
```

Or if your host is an IP address (note that, at the time of this writing, trogdord only supports IPV4):

```javascript
const connection = new Trogdord('192.168.0.1');
```

Finally, if trogdord is running on a non-standard port:

```javascript
const connection = new Trogdord('myhostname.com', 1041);
```

The constructor also takes a third optional argument for additional settings:

```javascript
// Connection attempt will timeout in 1 second instead of the default 3.
const connection = new Trogdord('myhostname.com', 1041, {
	connectTimeout: 1000
});
```

The following options are supported:

* **connectTimeout**: the number of milliseconds to wait while attempting to connect to trogdord before timing out

The trogdord module emits the following events while attempting to connect or when the connection is closed:

* **connect**: connection has been established
* **close**: connection has been closed
* **error**: an error occurred while attempting to connect

To make use of the connection after it's established, listen for the **connect** event:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// use the connection here
})
```

To handle errors, listen for the **error** event:

```javascript
const connection = new Trogdord();

connection.on('error', error => {

	// Uh oh!
	console.log(error);
})
```

If you want to trigger a block of code once the connection is closed, listen for the **close** event:

```javascript
const connection = new Trogdord();

connection.on('close', () => {

	// cleanup code goes here
})
```

### Retrieving Trogdord Statistics

This method retrieves useful statistical data about the instance of trogdord we're connected to:

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.statistics()
	.then(stats => {
		console.log(stats);
	}).catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
{
  players: 0,
  version: { major: 0, minor: 29, patch: 0 },
  lib_version: { major: 0, minor: 5, patch: 0 }
}
```

### Dumping Trogdord's State to Disk

This method tells trogdord to dump its state to disk, including all games, and
returns no output:

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.dump()
	.catch(error => {
		// ...Handle error...
	});
});
```

### Restoring Trogdord's State from Disk

This method tells trogdord to restore its state from disk, including all
previously dumped games, and returns no output:

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.restore()
	.catch(error => {
		// ...Handle error...
	});
});
```

### Retrieving Available Game Definitions

This method retrieves a list of all game definition files that are available to the server:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.definitions()
	.then(definitions => {
		console.log(definitions);
	}).catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
[ 'game.xml' ]
```

### Retrieving Games

This method retrieves a list of games that currently exist on the server:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.games()
	.then(games => {
		console.log(games);
	}).catch(error => {
		// ...Handle error...
	});
});
```

Result (an array of Game objects):

```
[ Game {} ]
```

You can also pass an optional list of filters to only return games matching certain criteria. For example, the following code returns only games that are currently running:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.games({is_running: true})
	.then(games => {
		// ...Do something with list of games...
	}).catch(error => {
		// ...Handle error...
	});
});
```

You can AND more filters together like the following example, which only returns games that are running and whose names start with the prefix "we":

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.games({is_running: true, name_starts: "we"})
	.then(games => {
		// ...Do something with list of games...
	}).catch(error => {
		// ...Handle error...
	});
});
```

If you need OR logic, you can pass in an array of filter groups like the following example, which returns all games that are running OR not running and start with the prefix "we":

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.games([{is_running: true}, {is_running: false, name_starts: "we"}]})
	.then(games => {
		// ...Do something with list of games...
	}).catch(error => {
		// ...Handle error...
	});
});
```

Currently supported filters for game lists:

* **is_running**: Takes a boolean value and returns games that are either running or not running
* **name_starts**: Takes a string value and returns games whose names start with the given value

### Retrieving a Single Game

This method retrieves the game corresponding to a specific id:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Retrieving game with id 0
	connection.getGame(0)
	.then(game => {
		console.log(game);
	}).catch(error => {
		// ...Handle error...
	});
});
```

Result (a Game object):

```
Game {}
```

### Creating a New Game

This method creates a new game:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.newGame('Game Name', 'game.xml')
	.then(game => {
		console.log(game);
	}).catch(error => {
		// ...Handle error...
	});
});
```

Result (a Game object):

```
Game {}
```

You can also set some initial metadata for the game:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.newGame('Game Name', 'game.xml', {metaKey1: 'value1', metaKey2: 'value2'})
	.then(game => {
		console.log(game);
	}).catch(error => {
		// ...Handle error...
	});
});
```

### Returning Game-Specific Statistics

Game.statistics() returns statistical data associated with a specific game.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and return its statistics
	connection.getGame(0)
	.then(game => game.statistics())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
{
  created: '2020-04-21 21:20:50 UTC',
  players: 1,
  current_time: 25,
  is_running: true
}
```

### Checking if a Game is Running

Game.isRunning() will return true if the game is running and false if not.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and check if it's running
	connection.getGame(0)
	.then(game => game.isRunning())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

### Getting Current In-Game Time

Game.getTime() will return the current in-time game.

Example:

```javascript
	// Get an existing game and check its current time
	connection.getGame(0)
	.then(game => game.getTime())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
```

Result (means the game timer has been running for 60 seconds):

```
60
```

### Starting a Game

Game.start() will start a game.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	let game;

	// Create a new game and then start it
	connection.newGame('Game Name', 'game.xml')
	.then(newGame => {
		game = newGame;
		return game.start();
	})
	.then(response => game.isRunning())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
true
```

### Stopping a Game

Game.stop() will stop a game.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	let game;

	// Get an existing game and stop it
	connection.getGame(0)
	.then(_game => {
		game = _game;
		return game.stop();
	})
	.then(response => game.isRunning())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
false
```

### Destroying a Game

Game.destroy() will destroy the game on the server side. Once the game has been destroyed, invoking further requests from the same object will result in 404 game not found errors.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and destroy it
	connection.getGame(0)
	.then(game => game.destroy())
	.then(response => {
		// ...Any code that needs to run after game has been destroyed...
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

### Getting a Game's Metadata

Game.getMeta() returns one or more metadata values associated with a game.

If you don't pass an argument, all metadata values will be returned.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and print all of its associated metadata
	connection.getGame(0)
	.then(game => game.getMeta())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
{ author: 'James Colannino', title: 'Super Funtime Sample Game', synopsis: "A rootin' tootin' good time!" }
```

You can also pass in specific metadata keys and only get those values back:

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and print some of its associated metadata
	connection.getGame(0)
	.then(game => game.getMeta(['author', 'title']))
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
{ author: 'James Colannino', title: 'Super Funtime Sample Game' }
```

Finally, if you only need a specific metadata value, you can skip the array and just pass in a string containing the desired key.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and print a metadata value
	connection.getGame(0)
	.then(game => game.getMeta('author'))
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
{ author: 'James Colannino' }
```

### Setting Game Metadata

Game.setMeta({key: value, ...}) takes as input an object of key, value pairs and sets them as metadata for the game.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game with id 0 and set some metadata
	connection.getGame(0)
	.then(game => game.setMeta({key1: 'value1', key2: 'value2'}))
	.then(response => {
		// ... do something once we know the metadata is set...
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

### Getting All Entities in the Game

Game.entities() will return all entities in the game. **Fun Fact**: Objects are called TObject because naming a JavaScript class "Object" doesn't work so well. Yes, I realized this the hard way.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and return all of its entities
	connection.getGame(0)
	.then(game => game.entities())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
[
  TObject {},  TObject {},
  TObject {},  TObject {},
  TObject {},  Room {},
  Room {},     Room {},
  Room {},     Creature {},
  Creature {}
]
```

If you'd like to get back an array of simple objects containing just the name and type of each entity, pass in false for the first optional argument.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and return all of its entities
	connection.getGame(0)
	.then(game => game.entities(false))
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
[
  { name: 'stick', type: 'object' },
  { name: 'boulder', type: 'object' },
  { name: 'rock', type: 'object' },
  { name: 'sword', type: 'object' },
  { name: 'candle', type: 'object' },
  { name: 'mysticalhall', type: 'room' },
  { name: 'start', type: 'room' },
  { name: 'chamber', type: 'room' },
  { name: 'cave', type: 'room' },
  { name: 'trogdor', type: 'creature' },
  { name: 'casper', type: 'creature' }
]
```

You can also return more specific lists of things. For example, to return all
entities that inherit from Thing:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and return all of its things
	connection.getGame(0)
	.then(game => game.things())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
[
  TObject {},  TObject {},
  TObject {},  TObject {},
  TObject {},  Creature {},
  Creature {}
]
```

And to just return all creatures:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and return all of its creatures
	connection.getGame(0)
	.then(game => game.creatures())
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
[
  Creature {},  Creature {}
]
```

Lists of all entity types can be requested.

### Getting a Specific Entity in a Game

Game.getEntity(name) returns a specific entity in the game.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get the room named 'start' from the game with id = 0
	connection.getGame(0)
	.then(game => game.getEntity('start'))
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
Room {}
```

You can also request more specific types of entities. For example, Game.getBeing(name) will return either a creature or a player by the given name. A method exists for each entity type.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get the TObject named 'stick' from the game with id = 0
	connection.getGame(0)
	.then(game => game.getObject('stick'))
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
Object {}
```

If an entity by the specified name exists in the game but does not match the requested type, an entity not found error will be returned.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// An entity named 'start' exists, but it's a room, not an object.
	connection.getGame(0)
	.then(game => game.getObject('start'))
	.then(response => {
		console.log(response);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:
```
Error: entity not found
    at node_modules/trogdord/lib/game.js:97:18
    at processTicksAndRejections (internal/process/task_queues.js:94:5) {
  status: 404
}
```

### Creating a New Player

Creating a new player in a game can be accomplished by a call to Game.createPlayer(name).

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Create a player in an existing game
	connection.getGame(0)
	.then(game => game.createPlayer('n00bslay3r'))
	.then(player => {
		console.log(player);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:
```
Player {}
```

### Retrieving Entity Output Messages

To retrieve the output messages for an entity (for example, a player), call Entity.output(channel).

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Retrieve a player's output on the 'display' channel
	connection.getGame(0)
	.then(game => game.getPlayer('n00bslay3r'))
	.then(player => player.output('display'))
	.then(output => {
		console.log(output);
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

Result:

```
[
  { timestamp: 1587598549, content: 'The Palace\n' },
  { timestamp: 1587598549, content: '\n' },
  {
    timestamp: 1587598549,
    content: "You're standing in the middle of a sprawling white marble castle, the walls lined with portraits of long deceased ancestors of some forgotten king.  Servants are bustling about, rushing around with steaming plates of meats and cooked vegetables, rushing in and out of rooms to serve princes and heads of state. Legend has it that going sideways can transport you to a strange and mystical land.\n" +
      '\n' +
      'To the north, you see a dark hole in the wall big enough to walk through.\n'
  },
  { timestamp: 1587598549, content: '\n' },
  { timestamp: 1587598549, content: 'You see a candle.\n' },
  { timestamp: 1587598549, content: '\n' },
  { timestamp: 1587598549, content: 'You see the Sword of Hope.\n' }
]
```

This will only work if the output driver trogdord is configured to use supports retrieving messages. If it doesn't (looking at you, redis driver), you'll get an error instead.

Result if trogdord is configured to use the redis output driver:

```
Error: redis output driver does not support this operation
    at node_modules/trogdord/lib/entity.js:106:19
    at processTicksAndRejections (internal/process/task_queues.js:94:5) {
  status: 501
}
```

### Appending Message to an Entity's Output Stream

Entity.output() can also append a new message to an Entity's output stream provided a second argument is specified.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Send a new message to a player's output stream
	connection.getGame(0)
	.then(game => game.getPlayer('n00bslay3r'))
	.then(player => {
		player.output('notifications', 'You were naughty. Be aware that further abuse will result in a permanent ban.');
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

### Sending a Command to a Player's Input Stream

Player.input(command) will send a command to the player's input stream. Return value will be a promise that resolves to a successful response object.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Retrieve a player from an existing game and send a command on its behalf
	connection.getGame(0)
	.then(game => game.getPlayer('n00bslay3r'))
	.then(player => {
		player.input('go north');
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

### Removing (Destroying) a Player

Removing a player can be accomplished with a call to Player.destroy(). Note that, at the time of this writing, no other entity type can be destroyed via a trogdord request.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	let player;

	// Remove an evil player from a game
	connection.getGame(0)
	.then(game => game.getPlayer('n00bslay3r'))
	.then(_player => {
		player = _player;
		return player.output('notifications', "You've been a bad monkey and are now banned.");
	})
	.then(response => {
		player.destroy();
	})
	.catch(error => {
		// ...Handle error...
	});
});
```

### Making a Raw JSON Request

Raw JSON requests are a low level mechanism that should, under ordinary circumstances, be made only by class methods whose underlying implementations are abstracted from the client. Nevertheless, you might run into a situation where making a raw request is advantageous or even necessary, and for this reason, the makeRequest method exists.

Example:

```javascript
const connection = new Trogdord()

connection.on('connect', () => {

	connection.makeRequest({
		method: "get",
		scope: "global",
		action: "statistics"
	}).then(response => {
		// ...Do something with the JSON response...
	}).catch(error => {
		// ...Handle error...
	});
});
```

By default, the request timeout is three seconds, but you can change that by passing in a different value (in milliseconds) as an optional second argument to makeRequest:

```javascript
const connection = new Trogdord()

connection.on('connect', () => {

	// Request times out in half a second instead of three
	connection.makeRequest({
		method: "get",
		scope: "global",
		action: "statistics"
	}, 500).then(response => {
		// ...
	}).catch(error => {
		// ...
	});
});
```
