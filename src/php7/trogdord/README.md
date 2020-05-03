# Trogdord Extension for PHP 7

The official PHP 7 client for trogdord.

## Installaton

To compile and install, issue the following commands:

```
phpize
./configure
make && make install
```

## Dependencies

* [Boost ASIO](https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio.html) >=1.60
* [RapidJSON](https://rapidjson.org/) >= 1.1.0

If you happen to be on an Ubuntu box (and possibly also on Debian), you can install these dependencies with the following command:

```
sudo apt-get install libboost-all-dev rapidjson-dev
```

The configure script doesn't currently check for these dependencies, but if either of these libraries are missing or are not in a place where the configure script can find them, the build will fail.

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

TODO: finish documentation