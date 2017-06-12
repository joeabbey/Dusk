# Dusk Game/Simulation Engine

[![Build Status](https://travis-ci.org/WhoBrokeTheBuild/Dusk.svg?branch=master)](https://travis-ci.org/WhoBrokeTheBuild/Dusk)
[![Build Status](https://ci.appveyor.com/api/projects/status/github/WhoBrokeTheBuild/Dusk?svg=true)](https://ci.appveyor.com/project/WhoBrokeTheBuild/dusk)

## Building

Dusk uses submodules for it's dependencies and example projects. After cloning,
run you need to initialize and pull the submodules with:

```
git submodule update --init
```

Other than submodules, it's a standard CMake build.

```
mkdir build && cd build
cmake ..
cmake --build .
```

## Running Examples

The examples are all made to run from wherever CMAKE_BINARY_DIR is. For easy of
use, there are targets provided to run the examples. These targets are "run-"
prefixed on the example name.

Example:
```
cmake --build . --target run-Cube
```
