# Artifact

## Overview

- A simple Lisp dialect
- Pure C++, zero dependencies

## Requirements

- C++17 compiler (hint: try `make clean; CXX=clang++-9 make;`)

## Build

### Release

  `make`

### Debug

  `make debug`

## Install

  `sudo make install`

## Run

### REPL

  `make run`

### From file

  `build/src/afct/afct <file>`

## Use

- Make sure to link in stdc++fs if you are linking against libartifact
