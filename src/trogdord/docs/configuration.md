# Configuration

All of trogdord's configuration takes place in `/etc/trogdord/trogdord.ini`. If you installed trogdord to a custom prefix, then this path will be relative to that location.

## Default trogdord.ini

Every new installation of trogdord includes the following default configuration file:

```ini
[network]

; By default, trogdord runs on port 1040.
; port=1040

; Socket option to allow the socket to be bound to an address that's already in
; use. If you don't enable this, you might have to wait a couple of minutes
; after stopping trogdord before you can start it again. On the flip side, not
; enabling it might in some circumstances be safer. For an explanation of what
; this is doing, see:
; https://stackoverflow.com/questions/3229860/what-is-the-meaning-of-so-reuseaddr-setsockopt-option-linux
; reuse_address=true

; Send TCP keep-alives (unless you know what you're doing, you'll want this
; option to remain on.)
; send_keepalive=true

[logging]

; Trogdord support 3 different logging methods: stdout (standard out), stderr
; (standard error), and file (in which case you will set the logto option to
; the filename where you'd like your logs to be appended.) By default, logto is
; set to stderr.
; logto=stderr
; logto=stdout
; logto=var/log/trogdord.log

[input]

; Input listeners receive strings from external sources asynchronously and feed
; them to the input driver. Zero or more input listeners can run at the same
; time. If redis support is enabled at compile-time, the redis input listener
; can be started by adding "redis" (the quotes are required) to the array of
; listeners below. The redis input listener can be further configured in the
; [redis] section. By default, no input listeners are configured.
; listeners=[]
; listeners=["redis"]

[output]

; This setting controls how messages in the output buffer are stored and
; retrieved. By default, the selected driver is set to "local", which manages
; messages locally inside trogdord. The other available driver is "redis", which
; sends output messages to a redis channel (if you're sending output messages
; to a websocket, this makes for quick and easy integration with servers like
; Socket.IO.)
; driver=local

; These settings will only be used if redis support was enabled at compile-time
; and if the redis output driver or input listener are used.
[redis]

; The hostname where the redis instance is running. Defaults to localhost.
; host=localhost

; The port the redis instance is listening on.
; port=6379

; The number of milliseconds we should wait when attempting to connect to Redis
; before timing out.
; connection_timeout=5000

; If a connection to redis fails, trogdord will automatically try to connect
; again after this number of milliseconds.
; connection_retry_interval=5000

; The redis channel we should publish output messages to.
; output_channel=trogdord:out

; The redis channel we should subscribe to in order to receive input
; input_channel=trogdord:in

[resources]

; This is the default path where game definition files are stored. Relative
; paths are based on the root directory where trogdord was installed.
; definitions_path=share/trogdor

```

To change an option from its default value, uncomment it by deleting the semi-colon, set it to the desired value, save the file, and restart trogdord.