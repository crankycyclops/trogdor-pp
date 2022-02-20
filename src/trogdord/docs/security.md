# Security

The purpose of trogdord is to host games and players, as well as to provide a simple interface for managing them. Since my preference is to develop small, modular tools that excel only at a particular task, and since trogdord is designed to be part of a higher level architecture with other tools that sit on top of it, I chose not to tackle the complex issue of security, at least in my initial implementation. Instead, care should be taken in how trogdord is setup so as to delegate those concerns to other parts of the system.

What follows are some considerations that should be made when setting up and running trogdord in a production environment. This is by no means an exhaustive list, but rather a high-level picture of what you should be thinking about when designing a larger architecture around trogdord.

If you're looking for an existing project that already builds on what trogdord provides, check out my sister project, [Trogserve](https://github.com/crankycyclops/trogserve). Trogserve is still in an early stage of development, but should give you an idea of how you might approach the additional tooling necessary for a complete multi-player experience.

## Accepting Connections

For security purposes, trogdord will listen only on 127.0.0.1 or ::1 by default. This setting can be altered to include additional whitelisted IPs, or can be set to listen on all interfaces via 0.0.0.0 or :: (this last option is not recommended for production, but can be useful when testing.)

## Placing Trogdord Behind a Firewall

Trogdord should **never** run on an open port on a public facing network. Since it will accept all connections without authentication and execute all valid requests, anyone in the world could maliciously delete or alter games and players or their data at any time. Instead, you should place it behind a firewall and setup your environment such that only trusted applications can communicate with it.

## Rate Limiting

Even if trogdord is hidden behind a firewall and can only be accessed by an approved client, it's still possible that the client itself could be used by someone on the outside as a vector for denial-of-service attacks. An effective way to combat this is to limit both the number of connections that can be made from a single endpoint and the number of requests that can be accepted in a given amount of time.

Trogdord does not currently do either of these things, so it's a good idea to configure your firewall and your front end code to ensure that neither of these become a problem.

I do plan on implementing some rudimentary support for these protections in a configurable way, but even so, it would be better to take care of this at a higher level before trogdord is ever reached.

## Administrative Privileges

Once you've taken the very important first step of blocking connections from the outside world, you should then make sure, when designing a system around trogdord, that only trusted clients and users can make privileged requests.

Once again, if you'd like to see how one might choose to control access to trogdord, you can refer to my sister project, [Trogserve](https://github.com/crankycyclops/trogserve). In this particular example, there's an admin panel accessible only to authenticated administrators. The admin panel is the only way that certain requests can be made, and as long as the environment in which Trogserve is running prevents access to trogdord from other clients, that should be sufficient to ensure that only the right people have full access.

You might envision an even more complex architecture in which different users have different privileges. How you structure access control to trogdord is entirely up to you.

## Player Accounts and Permissions

You might want to allow anyone to join any game under any name. Or, instead, you might want to restrict access to logged-in users. Perhaps you want to go one step further and only allow certain users to play certain games. By not taking user security into account, trogdord gives you the freedom to implement whatever authentication you choose (or to implement none at all.)

At a minimum, you'll likely want your architecture to prevent one player from altering another player's state.

## Other Considerations

As mentioned above, this is not an exhaustive list. If I have any additional ideas, I'll add them to this document, and, of course, if you'd like to make your own contribution, I welcome and encourage you to submit an issue or PR :)