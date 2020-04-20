# Trogdord Node.js Module

The official Node.js client for trogdord.

## Installaton

TODO: will publish to npm once I complete the first working version

## Dependencies

Requires Node.js version 12 or above.

## How to use

### Establishing a connection

By default, the Trogdord object will attempt to connect to localhost:1040 (1040 is the default port that trogdord runs on.) If those values are correct, then all you need to do is this:

```javascript
const connection = new Trogdord()
```

To specify a different hostname, pass it into the first argument of the constructor:

```javascript
const connection = new Trogdord('myhostname.com')
```

Or if your host is an IP address (note that, at the time of this writing, trogdord only supports IPV4):

```javascript
const connection = new Trogdord('192.168.0.1')
```

Finally, if trogdord is running on a non-standard port:

```javascript
const connection = new Trogdord('myhostname.com', 1041)
```

The constructor also takes a third optional argument for additional settings:

```javascript
// Connection attempt will timeout in 1 second instead of the default 3.
const connection = new Trogdord('myhostname.com, 1041, {
	connectTimeout: 1000
})
```

The following options are supported:

* **connectTimeout**: the number of milliseconds to wait while attempting to connect to trogdord before timing out

### Event Handling

The trogdord module emits the following events:

* **connect**: connection has been established
* **close**: connection has been closed
* **error**: an error occurred while attempting to connect

To use the connection, listen on the **connect** event:

```javascript
const connection = new Trogdord()

connection.on('connect', () => {

	// use the connection here
})
```

To handle errors, listen on the **error** event:

```javascript
const connection = new Trogdord()

connection.on('error', (error) => {

	// Uh oh!
	console.log(error) 
})
```

If you want to trigger a block of code once the connection is closed, listen on the **close** event:

```javascript
const connection = new Trogdord()

connection.on('close', () => {

	// cleanup code goes here
})
```

### Making a Raw Request

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