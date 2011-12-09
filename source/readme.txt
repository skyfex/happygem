HappyGem Source Code
====================

Directory structure
-------------------

drivers:
Everything that is specific to the hardware goes in here.
Every module on the hardware has to be abstracted into normal C functions.
Different hardware modules might require different drivers, which
is why they are seperated in folders. The simulator will also require
simulated drivers

services:
Services that build on top of the drivers to create higher-level
functionality that is shared across apps. For example a service
that keeps track of happy gems that are within range.

apps:
An app can be a game, some kind of utility (a clock, flashlight, etc),
or anything else that the user will interact with.

fw:
A collection of firmwares. A firmware is a complete software package
that can be uploaded to a happy gem. It integrates various apps, and
will also contain code deciding how/when to activate different apps.

sim:
A simulator used to discover how a collection of happygems will
work together.