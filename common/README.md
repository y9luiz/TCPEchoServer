# Common module

Here we have the code used for both (client and server).

Cypher Utils: This module holds all the business logic for encryption and decription.
I've decided to make it very simple, just a header file,to avoid extra complexity.

Messages: Here we have all data structure for messaging. The base Message class,holds a generic message implementation with header only and it child classes implement the body. I prefer to have a *.cpp and a .hpp file for each class, but for simplicity, I put everything in a single pair of files.
