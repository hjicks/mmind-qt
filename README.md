# Mastermind-qt
![an image of poor player, trying to guess the number](guess.ico)

MMind-qt is qt version of number guessing game, similar to [Mastermind board game](https://en.wikipedia.org/wiki/Mastermind_(board_game))

![demo](preview.gif)

The game starts with entering a length, and you a number of chances equal to it,
then, you must guess a number as long as that length. the game responds by X for incorrect digits and # for right ones.
The goal is to guess whole number completely before losing all of your chances.

## Installation

You may download binaries for windows, linux and openbsd from [releases page](://releases).

## Compiling

for compiling on windows you need qt, for OpenBSD you need `qt6-qtbase`, on Linux you need to have `qmake`, `qt` librares installed.

```
$ qmake
$ make
$ ./mastermind-qt
```
