# Methods, Scopes, and Actions

Every request that trogdord receives can be broken down into a method, a scope, and an action. These three values together map to a specific operation that trogdord should perform.

## Methods

The method component of a request is analagous to the HTTP methods GET, PUT, POST, DELETE, etc. and determines what sort of operation you wish to perform. The following methods have been defined and are understood by trogdord:

| | |
|-|-|
| **get** | These requests retrieve resources and data from the server and are analagous to the HTTP verb GET. |
| **post** | These requests create new resources and data and are analagous to the HTTP verb POST. |
| **put** | These requests replace already existing resources and data. |
| **set** | These requests are like HTTP's PATCH and modify (but don't replace) existing resources and data. |
| **delete** | These requests are like HTTP'S DELETE and destroy resources and data. |

## Scopes

TODO

## Actions

TODO

## A Complete Reference

Below, you'll find a complete reference of all scopes and actions in the current version of trogdord as well as how to format your requests.

TODO