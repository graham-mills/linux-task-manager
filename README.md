# Linux Task Manager

Experimenting with [Boost.Beast](https://github.com/boostorg/beast) to create a REST API for viewing Linux processes. The repo also includes a React based web app to query the server and display process information.

## Build Dependencies

For the API server:
- [CMake](https://cmake.org/)
- [Conan](https://conan.io/)
    * [fmt](https://conan.io/center/fmt)
    * [boost](https://conan.io/center/boost)
    * [nlohmann/json](https://conan.io/center/nlohmann_json)
    * [GTest](https://conan.io/center/gtest)

For the React app:

- [React](https://react.dev/)
- [Axios](https://axios-http.com/)
- [Jest](https://jestjs.io/)

## Building

1. In `/backend`, run `make install` and then `make build-release`
2. In `/frontend`, run `npm install` and then `npm run build`

## Running

With Docker:

1. Run `docker-compose up --build`

Without Docker:

1. In `/backend`, run `make serve`
2. In `/frontend`, run `npm run start`

Once running, browse to `http://localhost:3000` to view the React app or try one of the following endpoints:

- GET: `http://localhost:8080/api/uptime`
- GET: `http://localhost:8080/api/cpus`
- GET: `http://localhost:8080/api/mem`
- GET: `http://localhost:8080/api/procs`


