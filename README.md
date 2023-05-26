# Linux Task Manager

Experimenting with [Boost.Beast](https://github.com/boostorg/beast) to create a REST API for viewing Linux processes.

## Build Dependencies

* [CMake](https://cmake.org/)
* [Conan](https://conan.io/)
    * [fmt](https://conan.io/center/fmt)
    * [boost](https://conan.io/center/boost)
    * [nlohmann/json](https://conan.io/center/nlohmann_json)
    * [GTest](https://conan.io/center/gtest)

## Development Dependencies

* clang-format
* cppcheck

## Docker

A Dockerfile is provided to build and run the server within a separate dev environment, without needing to install any of the build dependencies directly.

* Run `make docker-build` to build the repo
* Run `make docker-serve` to run the server

Once the server is running, try the following endpoints:

- GET:`http://localhost:8080/v0/uptime`
- GET:`http://localhost:8080/v0/cpus`
- GET:`http://localhost:8080/v0/mem`
- GET:`http://localhost:8080/v0/procs`
