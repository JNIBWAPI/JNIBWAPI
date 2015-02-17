# JNIBWAPI

Build a [Starcraft](http://us.blizzard.com/en-us/games/sc/) AI in Java!

This project provides a Java interface for the Brood War API ([BWAPI](http://bwapi.github.io/)), using Java Native Interface ([JNI](https://en.wikipedia.org/wiki/Java_Native_Interface)) to communicate over a shared memory bridge. This allows Java developers to write AI for Starcraft, one of the most popular [Real-Time Strategy](https://en.wikipedia.org/wiki/Real-time_strategy) games, and participate in AI competitions (see links below).

## Download

[Download JNIBWAPI releases](https://github.com/JNIBWAPI/JNIBWAPI/releases)

## Getting Started / Documentation

Take a look at the [wiki](https://github.com/JNIBWAPI/JNIBWAPI/wiki).

## Updates

* 2015-02-11: JNIBWAPI has moved to GitHub
* 2014-05-13: JNIBWAPI 1.0 is released. Includes significant changes to the API to bring it closer to the C++ API and encourage type safety.

## Developers

Interested in improving the project? There are several areas for improvement including:

* Improving the message passing protocol between the C++ and Java code
* Exposing additional BWAPI functions to Java agents
* Testing and documentation 

If you have recommendations for improving the project or would like to help contribute, then feel free to use the issue tracker or submit a pull request.

## Links

* [Starcraft](http://us.blizzard.com/en-us/games/sc/)
* [Brood War API](https://bwapi.github.io/)
* [BWMirror (alternative to JNIBWAPI)](http://bwmirror.jurenka.sk/)
* [AIIDE StarCraft AI Competition](http://www.starcraftaicompetition.com/)
* [CIG StarCraft AI Competition (2013)](http://ls11-www.cs.uni-dortmund.de/rts-competition/starcraft-cig2013)
* [Starcraft Micro AI Competition](http://scmai.hackcraft.sk/)
* [Student StarCraft AI Competition](http://www.sscaitournament.com/)
* [Starcraft Brood War Bots Ladder](http://bots-stats.krasi0.com/)
* [Competition news](https://twitter.com/StarCraftAIComp)
* [JBridge (deprecated)](https://code.google.com/p/bwapi-jbridge/)
* [Java Proxy (deprecated)](https://code.google.com/p/bwapi-proxy/)

## Legal

[Starcraft](http://us.blizzard.com/en-us/games/sc/) and  Starcraft: Brood War are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries. ©1998 Blizzard Entertainment, Inc. All rights reserved.

[BWAPI](https://bwapi.github.io/) is a third party "hack" that violates the End User License Agreement (EULA). It is strongly recommended to purchase a legitimate copy of Starcraft: Broodwar from Blizzard Entertainment before using BWAPI. 

This code is available under the LGPL
