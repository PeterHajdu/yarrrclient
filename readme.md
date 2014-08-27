# Introduction

Yarrr is the most awesome space pirate mmorpg game ever made.

# Planned platforms

Almost certain:
 * freebsd
 * linux
 * android

Packages are going to be maintained for:
 * arch
 * ubuntu
 * freebsd

Maybe:
 * windows
 * iphone

# Installation

## arch

Add the following few lines to your /etc/pacman.conf

```
[yarrr-testing]
SigLevel = Optional TrustAll
Server = http://repo.yarrrthegame.com/arch/$repo/os/$arch
```

## ubuntu

Add the following few lines to your /etc/apt/sources.list

```
deb http://repo.yarrrthegame.com/debian trusty multiverse
```

