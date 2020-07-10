# Security

The purpose of trogdord is to host games and players as well as to provide a simple interface to manage them. Since my preference is to develop small, modular tools that excel only at a particular task, and since trogdord is designed to be part of a higher level architecture with other tools that sit on top of it, I chose not to tackle the complex issue of security, at least in my initial implementation. Instead, care should be taken in how trogdord is setup so as to delegate those security concerns to other parts of the system that use it.

What follows are some considerations that should be taken into account when setting up and running trogdord in a production environment. This is by no means an exhaustive list, but rather a high-level picture of the things you should be thinking about when designing an architecture around trogdord.

If you're looking for an existing project that already builds on what trogdord provides, check out my sister project, [Trogserve](https://github.com/crankycyclops/trogserve). Trogserve is still in an early stage of development, but should give you an idea of how you might approach the additional tooling necessary for a complete multi-player experience.

## Placing Trogdord Behind a Firewall

Trogdord should **never** run on an open port on a public facing network. Since it will accept all connections and execute all valid requests, anyone in the world could maliciously delete or alter games and players at any time. Instead, you should place it behind a firewall and setup your environment such that only trusted tools can access it.

## Administrative Privileges

Once you've taken the very important first step of blocking connections from the outside world, you should make sure when designing a system around trogdord that only trusted clients and users can make privileged requests.

Once again, if you'd like to see how one might choose to implement such privileges, you can refer to my sister project, [Trogserve](https://github.com/crankycyclops/trogserve). In this particular example, there's an admin panel accessible only to privileged logged-in users. The admin panel is the only way to make certain requests, and as long the environment in which Trogserve is running prevents access to trogdord from outside sources, that should be sufficient to ensure that only trusted administrators can make sensitive requests via the admin panel.

You might envision a more complex architecture in which different users have privileges to execute different kinds of requests. How you delegate those privileges is up to you and is a decision you'll have to make on your own.

## Player Accounts and Permissions

You might want to allow anyone to join any game under any name. Or, instead, you might want to restrict access to logged in users. Perhaps you want to go one step further and only allow certain users to play certain games. By not taking user security into account, trogdord gives you the freedom to implement whatever authentication methods you choose (or to implement none at all if you'd like the games you host to remain open.)

## Other Considerations

As mentioned above, this is not meant to be an exhaustive list. If I have any additional ideas for this document, I'll add them, and, of course, if you have ideas of your own for how this might be improved, I welcome and encourage you to submit an issue or a PR :)