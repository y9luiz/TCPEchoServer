# TCP echo server

This project aims to create a TCP echo server, where clients can authenticate themselves. The server listen to connections and for each new connection, the server will validade the username and password, once the login is succeed, the server will wait for an echo request, then it will reply back with an echo response.