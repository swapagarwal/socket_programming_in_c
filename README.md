# Client-Server programming using both TCP and UDP sockets

The aim is to implement a simple 2 stage communication protocol.

Initially, server waits for a TCP connection from the client. Then, client connects to
the server using server’s TCP port already known to the client. After successful connection, the
client sends a Request Message (Type 1 message) to the server via TCP port to request a UDP port
from server for future communication. After receiving the Request Message, server obtains a UDP
port number and sends this port number back to the client as a Response Message (Type 2
Message) over the TCP connection. After this negotiation phase, the TCP connection on both the
server and client is closed gracefully releasing the socket resource.

In the second phase, the client transmits a short Data Message (Type 3 message) over the negotiated
UDP port. The server displays the received Data Message and sends a Data Response (type 4
message) to indicate the successful reception. After this data transfer phase, both sides close their
UDP sockets.

The server is implemented as a "Concurrent Server", i.e., a server that accepts connections from
multiple clients and serves all of them concurrently.

### Usage

**Client:** ```<executable code> <Server IP Address> <Server Port number>```

**Server:** ```<executable code> <Server Port number>```

![Client](https://github.com/swapagarwal/socket_programming_in_c/blob/master/client.gif)

![Server](https://github.com/swapagarwal/socket_programming_in_c/blob/master/server.gif)
