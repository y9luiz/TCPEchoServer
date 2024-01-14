# TCP Echo Server

This project implements a TCP echo server with two main steps:

- **Authentication**: The server verifies if the client is a known client.
- **Validation**: The client sends an encrypted message to the server and expects the decrypted response to validate its identity.

## Authentication

The server maintains an in-memory user database. When a client attempts to connect, the server receives a login request. The login request is a structured message composed of a message header, a username, and a password.

The message header is structured as follows:
```cpp
header: {
    MessageType: 1 byte, // message type identifier
    MessageLen: 2 bytes, // total size of the message (header + body)
    SequenceId: 1 byte // message sequence identifier
} // 4 bytes
```

so for login request we have:
```cpp
// MessageType = 0
LoginRequestMessage : {
    Header: 4 bytes
    username: 32 bytes,
    password: 32 bytes
} // 68 bytes
```

Once the authenticate is performed, the server will reply with a login response message, with the following shape:

```cpp
// MessageType = 1
LoginResponseMessage : {
    Header: 4 bytes
    StatusCode: 2 bytes, // 0=Failed, 1=OK
} // 6 bytes
```

## Validation

After conclude the authentication flow, we move for validation.

In this step, the server will get a cryptografed message. And will decrypt it and send it back to the client, so the client can validate it integrity.

```cpp
// MessageType = 2

EchoRequest : {
    Header: 4 bytes,
    MessageSyze: 2 bytes,
    EncryptedMessage: variable, // byte sequence with variable size, determinated by MessageSyze
}
```

```cpp
// MessageType = 3
EchoResponse : {
    Header: 4 bytes,
    MessageSyze: 2 bytes,
    Message: variable, // byte sequence with variable size, determinated by MessageSyze
}
```

# How to build this project?

I've written this code using CMake and Boost library as backend interface, so it works in any platform.

To compile it, follow these steps:

## Linux
For compile it on linux, you have two options.
- Using conan (recommended)
- Using your package manager

I recommend the first one (conan), because conan will not install any dependencie inside of your OS itself.

### Compiling using conan
I'll assume in the next steps that you're using a debian based system. If you are using other distro, that's fine, just use your own package manager.

> sudo apt-get update

> sudo apt-get install build-essential python3 python3-pip cmake ninja-build

> pip install --user conan

> source ~/.profile

> conan profile detect --force

> cd `<this project folder>`

> conan install --output-folder=build

> cd build

> cmake .. -G Ninja -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

> cmake --build .

We're done!

### Compiling without conan

> sudo apt-get update

> sudo apt-get install build-essential libboost-all-dev

> cd `<this project folder>`

> mkdir build

> cd build

> cmake ..

> ninja

That's it, you probably going see two executables into your build folder:
- echo_server: Echo server implementation, for more details, look into server folder
- client_example: Client example, for more details, look into client folder.

## Using docker

Build a image using the dockerfile

> docker build -t tcp_echo_server .

Now it's just plug an play.

For running the server:

> docker run -p 5000:500 tcp_echo_server tcp_echo_server echo_server

And run the client:
> docker run -p 5000:5000 tcp_echo_server client_example