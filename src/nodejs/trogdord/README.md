# Trogdord Node.js Module

The official Node.js client for trogdord.

## Installaton

TODO: will publish to npm once I complete the first working version

## Dependencies

Requires Node.js 12.0 or above.

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
const connection = new Trogdord('myhostname.com, 1041, {
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

connection.on('error', (error) => {

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
	.then((stats) => {
		console.log(stats);
	}).catch((error) => {
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

### Retrieving Available Game Definitions

This method retrieves a list of all game definition files that are available to the server:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.definitions()
	.then((definitions) => {
		console.log(definitions);
	}).catch((error) => {
		// ...Handle error...
	});
});
```

Result:

```
[ 'game.xml' ]
```

### Retrieving All Games

This method retrieves all games that currently exist on the server:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.games()
	.then((games) => {
		console.log(games);
	}).catch((error) => {
		// ...Handle error...
	});
});
```

Result (an array of Game objects):

```
[ Game {} ]
```

### Retrieving a Single Game

This method retrieves the game corresponding to a specific id:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Retrieving game with id 0
	connection.getGame(0)
	.then((game) => {
		console.log(game);
	}).catch((error) => {
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
	.then((game) => {
		console.log(game);
	}).catch((error) => {
		// ...Handle error...
	});
});
```

Result (a Game object):

```
Game {}
```

You can also set some initial meta data for the game:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	connection.newGame('Game Name', 'game.xml', {metaKey1: 'value1', metaKey2: 'value2'})
	.then((game) => {
		console.log(game);
	}).catch((error) => {
		// ...Handle error...
	});
});
```

### Checking if a Game is Running

Game.isRunning() will return true if the game is running and false if not.

Example:

```javascript
const connection = new Trogdord();

connection.on('connect', () => {

	// Get an existing game and check if it's running
	connection.getGame(0)
	.then((game) => {
		return game.isRunning();
	})
	.then((response) => {
		console.log(response);
	})
	.catch((error) => {
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
	.then((game) => {
		return game.getTime();
	})
	.then((response) => {
		console.log(response);
	})
	.catch((error) => {
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
	.then((newGame) => {
		game = newGame;
		return game.start();
	})
	.then((response) => {
		return game.isRunning();
	})
	.then((response) => {
		console.log(response);
	})
	.catch((error) => {
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
	.then((_game) => {
		game = _game;
		return game.stop();
	})
	.then((response) => {
		return game.isRunning();
	})
	.then((response) => {
		console.log(response);
	})
	.catch((error) => {
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

	let game;

	// Get an existing game and destroy it
	connection.getGame(0)
	.then((game) => {
		return game.destroy();
	})
	.then((response) => {
		// ...Any code that needs to run after game has been destroyed...
	})
	.catch((error) => {
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
	}).then((response) => {
		// ...Do something with the JSON response...
	}).catch((error) => {
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
	}, 500).then((response) => {
		// ...
	}).catch((error) => {
		// ...
	});
});
```