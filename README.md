# Artifact

## Overview

- A simple Lisp dialect
- Includes a an expression builder and querier

## Requirements

- C++17 compiler (hint: try `make clean; CXX=clang++-9 make;`)
- Boost Test (`libboost-test-dev`)

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

## Test

  `make test`
