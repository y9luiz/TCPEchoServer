# Server

I've invested more time in this module.

To start we have our main.cpp file which is the entrypoint for the server.

You should start the server and then press `a` to add a new user or `q` for quit.

The actual implementation of this servers users a pointer implementation design patter. I decided to do it, just because I would not like that the code that will consume this class actually really care about boost, for me this should be a hidden thing. I didn't have the same care with the client, just because the client is just a small example.

## Why boost?

Boost is a library that runs in any platform (linux, windows, mac) and has a lot of useful data structures, and more important: boost has asio, which makes very easy to implement asynchronous communication.

## Why connection is a subdirectory of TCP Server?

Well, connection class is a hidden class. I don't wanna expose it to the application.
If I put it in the tcp_server folder, the application will see it header file (connection.hpp).
So I've prefer put into it own subdirectory.