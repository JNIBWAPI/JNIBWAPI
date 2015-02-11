# JNI-BWAPI

Build a [StarCraft](http://us.blizzard.com/en-us/games/sc/) AI in Java!

## Download

[Download JNIBWAPI](https://drive.google.com/a/soe.ucsc.edu/folderview?id=0Bxdd3L2Qj5lxamZUSUpOLVZ6c2M) ([older versions](https://code.google.com/p/jnibwapi/downloads/list))

## Updates

* 2015-02-11: JNIBWAPI has moved to GitHub
* 2014-05-13: JNIBWAPI 1.0 is released. Includes significant changes to the API to bring it closer to the C++ API and encourage type safety.

## Overview

This project provides a Java interface for the Brood War API ([BWAPI](http://bwapi.github.io/)) using Java Native Interface ([JNI](https://en.wikipedia.org/wiki/Java_Native_Interface)) and a shared memory bridge. Compatible with BWAPI 3.7 and available under the LGPL. This allows Java developers participate in Starcraft AI competitions such as [AIIDE](http://www.starcraftaicompetition.com/), [CIG](http://ls11-www.cs.uni-dortmund.de/rts-competition/starcraft-cig2013), [SCMAI](http://scmai.hackcraft.sk/), and [SSCAI](http://www.sscaitournament.com/).

![Overview diagram JNIBWAPI connection to Starcraft over shared memory bridge](http://i.imgur.com/LWzam6k.png "Overview")

The JNI interface provides several advantages over the previous socket-based [ProxyBot](http://eis.ucsc.edu/StarCraftRemote) interface. First, the interface uses the BWAPI shared memory bridge which lessens the communication bottleneck between C++ and Java and prevents cheating using direct memory access. Second, the BWAPI utility functions (e.g. canBuildHere) can be now be called from Java. Finally, the use of JNI should result in easier project maintainability and extensibility.

## Getting Started

Take a look at the [getting started guide](https://code.google.com/p/jnibwapi/wiki/GettingStarted). There is also a [short demo video](https://www.youtube.com/watch?v=4yUy7j7skRQ) of the example 5-pool bot included in JNIBWAPI.

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
* [Competition news](https://twitter.com/StarCraftAIComp)
* [JBridge (deprecated)](https://code.google.com/p/bwapi-jbridge/)
* [Java Proxy (deprecated)](https://code.google.com/p/bwapi-proxy/)

## Legal

[Starcraft](http://us.blizzard.com/en-us/games/sc/) and  Starcraft: Brood War are trademarks or registered trademarks of Blizzard Entertainment, Inc. in the U.S. and/or other countries. ©1998 Blizzard Entertainment, Inc. All rights reserved.
[BWAPI](https://bwapi.github.io/) is a third party "hack" that violates the End User License Agreement (EULA). It is strongly recommended to purchase a legitimate copy of Starcraft: Broodwar from Blizzard Entertainment before using BWAPI. 
