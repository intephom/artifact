# Artifact

## Overview

A simple Lisp dialect

- Closures
- Tail-call optimisation
- Hash table #(syntactic sugar)
- An expression builder, querier and visitor
- An interface for pluggable native functions

## Requirements

### Compiler

- C++20 compiler (hint: try `make clean; CXX=g++-9 make;`)

### Libraries

- Boost Container (`libboost-container-dev`)
- Boost Test (`libboost-test-dev`)
- fmt (`libfmt-dev`)

### Tools

- `rlwrap` (optional) for running the REPL

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
