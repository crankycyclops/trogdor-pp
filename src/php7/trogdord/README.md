![PHP Tests](https://github.com/crankycyclops/trogdor-pp/workflows/PHP%20Tests/badge.svg)

# Trogdord Extension for PHP 7

The official PHP 7 client for trogdord.

## Dependencies

* [PHP](https://www.php.net/) 7.2, 7.3, or 7.4, along with the include files and access to phpize
* [Boost ASIO](https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio.html) >=1.60
* [RapidJSON](https://rapidjson.org/) >= 1.1.0

If you happen to be on an Ubuntu box (and possibly also on Debian), you can install these dependencies with the following command:

```
sudo apt-get install php7.x-dev libboost-system-dev rapidjson-dev (where 7.x is your desired php version)
```

The configure script doesn't currently check for the existence of Boost ASIO or RapidJSON, but if either of these libraries are missing or are not in a place where the configure script can find them, the build will fail.

## Installaton

To compile and install, issue the following commands:

```
phpize
./configure
make && make install
```

## How to use

### Establishing a Connection

If trogdord is running on 1040, the default port, you can connect by instantiating the `\Trogdord` class with the hostname as the only argument. If an error occurs while attempting to connect, an instance of `\Trogdord\NetworkException` will be thrown.

```php
try {
	$connection = new \Trogdord("localhost");
}

catch (\Trogdord\NetworkException $e) {
	// Handle error
}
```

If trogdord is running on a non-standard port, you can pass that port number in as an optional second argument:

```php
try {
	// Trogdord is running on port 1041 instead of the default 1040
	$connection = new \Trogdord("localhost", 1041);
}

catch (\Trogdord\NetworkException $e) {
	// Handle error
}
```

### Retrieving Trogdord Statistics

`\Trogdord::statistics()` retrieves useful statistical data about the instance of trogdord we're connected to:

```php
try {
	$connection = new \Trogdord("localhost");
	$statistics = $connection->statistics();

	var_dump($statistics);
}

catch (\Trogdord\NetworkException $e) {
	// Handle error
}
```

Result:

```
array(3) {
  ["lib_version"]=>
  array(3) {
    ["major"]=>
    int(0)
    ["minor"]=>
    int(6)
    ["patch"]=>
    int(0)
  }
  ["players"]=>
  int(0)
  ["version"]=>
  array(3) {
    ["major"]=>
    int(0)
    ["minor"]=>
    int(33)
    ["patch"]=>
    int(2)
  }
}
```

Like `\Trogdord`'s constructor, `\Trogdord::statistics()` will throw an instance of `\Trogdord\NetworkException` if there are any issues with the connection.

### Retrieving Games

`\Trogdord::games()` retrieves a list of games that currently exist on the server:

```php
try {
	$connection = new \Trogdord("localhost");
	$statistics = $connection->games();

	var_dump($games);
}

// A connection-related error occurred
catch (\Trogdord\NetworkException $e) {
	// Handle error
}

// The connection is fine, but something went wrong with the request
// ($e->getCode() will provide the trogdord request status and $e->getMessage()
// will explain what went wrong.)
catch (\Trogdord\RequestException $e) {
	// Handle error
}
```

Result:

```
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    int(0)
    ["name"]=>
    string(5) "game1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(1)
    ["name"]=>
    string(5) "game2"
  }
  [2]=>
  array(2) {
    ["id"]=>
    int(2)
    ["name"]=>
    string(5) "game3"
  }
  [3]=>
  array(2) {
    ["id"]=>
    int(3)
    ["name"]=>
    string(13) "mystartedgame"
  }
  [3]=>
  array(2) {
    ["id"]=>
    int(3)
    ["name"]=>
    string(13) "mystoppedgame"
  }
}
```

You can also pass an optional list of filters to only return games matching certain criteria. For example, the following code returns only games that are currently running:

```php
$startedGames = $connection->games([
	'is_running' => true
]);

var_dump($startedGames);
```

Result:

```
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    int(0)
    ["name"]=>
    string(5) "game1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(3)
    ["name"]=>
    string(13) "mystartedgame"
  }
}
```

You can AND more filters together like the following example, which only returns games that are running and whose names start with the prefix "my":

```php
$startedGames = $connection->games([
	'is_running' => true,
	'name_starts' => 'my'
]);

var_dump($startedGames);
```

Result:

```
array(1) {
  [0]=>
  array(2) {
    ["id"]=>
    int(3)
    ["name"]=>
    string(13) "mystartedgame"
  }
}
```

If you need OR logic, you can pass in an array of filter groups like the following example, which returns all games that are running OR not running and start with the prefix "my":

```php
$games = $connection->games([[
	'is_running' => true
], [
	'is_running' => false,
	'name_starts' => 'my'
]]);

var_dump($games);
```

Result:

```
array(3) {
  [0]=>
  array(2) {
    ["id"]=>
    int(0)
    ["name"]=>
    string(5) "game1"
  }
  [1]=>
  array(2) {
    ["id"]=>
    int(3)
    ["name"]=>
    string(13) "mystartedgame"
  }
  [2]=>
  array(2) {
    ["id"]=>
    int(4)
    ["name"]=>
    string(13) "mystoppedgame"
  }
}
```

Currently supported filters for game lists:

* **is_running**: Takes a boolean value and returns games that are either running or not running
* **name_starts**: Takes a string value and returns games whose names start with the given value

If for any reason you need to retrieve one or more meta values associated with the games in the same request (if the value doesn't exist for a particular game, you'll just get an empty string), you can add an array of those meta keys as an optional second argument to `\Trogdord::games()`:

```php
// We want to retrieve all games along with the meta values "title" and "author"
$games = $connection->games(null, ['title', 'author']);

var_dump($games);
```

Result:

```
array(5) {
  [0]=>
  array(4) {
    ["id"]=>
    int(0)
    ["name"]=>
    string(5) "game1"
    ["title"]=>
    string(14) "Game 1's title"
    ["author"]=>
    string(15) "James Colannino"
  }
  [1]=>
  array(4) {
    ["id"]=>
    int(1)
    ["name"]=>
    string(5) "game2"
    ["title"]=>
    string(14) "Game 2's title"
    ["author"]=>
    string(15) "James Colannino"
  }
  [2]=>
  array(4) {
    ["id"]=>
    int(2)
    ["name"]=>
    string(5) "game3"
    ["title"]=>
    string(14) "Game 3's title"
    ["author"]=>
    string(15) "James Colannino"
  }
  [3]=>
  array(4) {
    ["id"]=>
    int(3)
    ["name"]=>
    string(10) "mystartedgame"
    ["title"]=>
    string(23) "My Started Game's title"
    ["author"]=>
    string(15) "James Colannino"
  }
  [4]=>
  array(4) {
    ["id"]=>
    int(4)
    ["name"]=>
    string(13) "mystoppedgame"
    ["title"]=>
    string(23) "My Stopped Game's Title"
    ["author"]=>
    string(15) "James Colannino"
  }
}
```

### Retrieving Available Game Definitions

`\Trogdord::definitions()` returns a list of all game definition files available to the server.

```php
try {
	$connection = new \Trogdord("localhost");
	$definitions = $connection->definitions();

	var_dump($definitions);
}

catch (\Trogdord\NetworkException $e) {
	// Handle connection-related error
}

catch (\Trogdord\RequestException $e) {
	// Handle request-related error
}
```

Result:

```
array(1) {
  [0]=>
  string(9) "game1.xml"
  [1]=>
  string(9) "game2.xml"
  [2]=>
  string(9) "game3.xml"
}
```

### Retrieving a Single Game

`\Trogdord::getGame(id)` retrieves a single game from trogdord if it exists and
throws an instance of `\Trogdord\GameNotFound` if not.

```php
try {

	$connection = new \Trogdord("localhost");
	$game = $connection->getGame(0);

	var_dump($game);
}

catch (\Trogdord\NetworkException $e) {
	// Handle connection-related error
}

catch (\Trogdord\GameNotFound $e) {
	// Game doesn't exist
}
```

Result:

```
object(Trogdord\Game)#3 (4) {
  ["name":"Trogdord\Game":private]=>
  string(6) "mygame"
  ["definition":"Trogdord\Game":private]=>
  string(8) "game.xml"
  ["id":"Trogdord\Game":private]=>
  int(0)
  ["trogdord":"Trogdord\Game":private]=>
  object(Trogdord)#1 (0) {
  }
}
```

Note that `\Trogdord\Game` contains an instance of the connection that spawned it. The actual property is private, but you can access it through `\Trogdord\Game::__get`. Thus, if you need to, you can use it as in the following example:

```php
$game->trogdord->statistics();
```

The name and id are also available as read-only properties, courtesy of `\Trogdord\Game::__get`:

```php
var_dump($game->name);
var_dump($game->id);
```

Result:

```
string(6) "mygame"
int(0)
```

### Creating a New Game

`\Trogdord::newGame(name, definition)` creates a new game with the specified name and based
on the passed definition file (must be one of the definitions available to the
connected instance of trogdord) and returns an instance of \Trogdord\Game.
Throws an instance of `\Trogdord\NetworkException` if an issue arises with the
connection and an instance of `\Trogdord\RequestException` if anything goes
wrong with the request (for example, if the game definition filename passed in
isn't available to the server.)

Example:

```php
try {

	$connection = new \Trogdord("localhost");
	$game = $connection->newGame('mycoolgame', 'game.xml');

	var_dump($game);
}

catch (\Trogdord\NetworkException $e) {
	// Handle error
}

catch (\Trogdord\RequestException $e) {
	// Handle error
}
```

Result:

```
object(Trogdord\Game)#3 (4) {
  ["name":"Trogdord\Game":private]=>
  string(10) "mycoolgame"
  ["definition":"Trogdord\Game":private]=>
  string(8) "game.xml"
  ["id":"Trogdord\Game":private]=>
  int(0)
  ["trogdord":"Trogdord\Game":private]=>
  object(Trogdord)#1 (0) {
  }
}
```

### Returning Game-Specific Statistics

TODO

### Checking if a Game is Running

TODO

### Getting Current In-Game Time

TODO

### Starting a Game

TODO

### Stopping a Game

TODO

### Destroying a Game

TODO

### Getting a Game's Metadata

TODO

### Setting Game Metadata

TODO

### Getting All Entities in the Game

TODO

### Getting a Specific Entity in a Game

TODO

### Creating a New Player

TODO

### Retrieving Entity Output Messages

TODO

### Appending Message to an Entity's Output Stream

TODO

### Removing (Destroying) a Player

TODO

### Sending a Command to a Player's Input Stream

TODO

## Classes

The `trogdord` extension defines the following classes:

### \Trogdord

Represents a connection to trogdord and is used to create and retrieve games, as
well as to return statistical information about the server.

Below is a complete list of public methods (all methods including the constructor
throw an instance of `\Trogdord\NetworkException` if anything goes wrong with the
connection):

* `__construct(string hostname, int port = 1040)` — Establish a connection to an
instance of trogdord over the network.

* `statistics(): array` — Return useful statistics about the instance of trogdord.
Throws `\Trogdord\RequestException` if anything goes wrong with the request (you
shouldn't ever get this exception from this method unless there's a bug in the
server or the extension.)

* `games(array filters = null, array metaKeys = null): array` — Returns a list of
games that exist on the server. Filters can be used to only include games matching
certain criteria (see examples above) and metaKeys can be used to include certain
meta data values for each game in the list. Throws `\Trogdord\RequestException`
if anything goes wrong with the request (you shouldn't ever get this exception
from this method unless there's a bug in the server or the extension.)

* `definitions(): array` — Returns a list of all game definition files available
to the server. `\Trogdord\RequestException` if anything goes wrong with the
request (you shouldn't ever get this exception from this method unless there's a
bug in the server or the extension.)

* `getGame(int id): \Trogdord\Game` — Returns an instance of `\Trogdord\Game`
representing the game with the specified id. Throws `\Trogdord\GameNotFound` if
the game with the specified id doesn't exist.

* `newGame(string name, string definition): \Trogdord\Game` — Creates a new game
and returns an instance of `\Trogdord\Game` that represents it. Throws
`\Trogdord\RequestException` if anything goes wrong with the request.

### \Trogdord\Game

TODO

### \Trogdord\Entity

TODO

### \Trogdord\Place

TODO

### \Trogdord\Thing

TODO

### \Trogdord\Being

TODO

### \Trogdord\Room

TODO

### \Trogdord\Object

TODO

### \Trogdord\Creature

TODO

### \Trogdord\Player

TODO

## Exceptions

The `trogdord` extension throws various exceptions, all of which are outlined here:

* `\Trogdord\Exception` — All exceptions inherit from this one. If you want to
write a single block that will capture every possible error, this is the type
you'd want to catch.

* `\Trogdord\FilterException` — If you attempt to pass invalid filters to a
method that accepts them (for example, `\Trogdord::games`), this exception will
be thrown. This inherits from `\Trogdord\Exception`.

* `\Trogord\NetworkException` — This is thrown whenever an error occurs with the
underlying TCP connection. For example, if we attempt to start or stop a game
but trogdord has gone down and the extension can't connect to it, you'll see
this exception. This inherits from `\Trogdord\Exception`.

* `\Trogdord\RequestException` — You'll see this whenever a non-connection-related
exception occurs while making a request to trogdord. For example, if you attempt
to start a game that no longer exists, an instance of `\Trogdord\GameNotFound`
will be thrown, which inherits from this. Includes both a message and a code
(the code in this case is the status code that comes back in trogdord's response.)
This inherits from `\Trogdord\Exception`.

* `\Trogdord\GameNotFound` — If you attempt to call a method on an instance of
`\Trogdord\Game` representing a game that no longer exists, this exception will
be thrown. This inherits from `\Trogdord\RequestException`.

TODO: finish listing remaining exceptions