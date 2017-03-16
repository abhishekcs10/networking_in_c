======================
Assignment 2 Report
======================

Approach
---------

The server creates a shared memory where the information about the clients and messages to be delivered are queued.
A file is created for storing the logs.
The server then forks a process which waits for any message to be entered into queue. The process then delivers the message to the process' shared memory buffer which is forked corresponding to the every new socket created when the client connects and deliver message to it.
At the client side there are two processes forked one for reading the buffer and another for writing. The client process thus can simultaneously read and write to the server for processing.

Boundary cases
----------------

1- Message to self (loopback address) not allowed.
2- If the client sends message in incorrect format it will be notified by the server.
3- If more than five clients connect to the server the connection will be terminated for new clients.
4- If user gets disconnected, while other user is sending message, it will be notified.
5- The message to all user will be shown if some user got disconnected.
6- any wrong user id or user name will be reported.
7- If server's message buffer gets full the user will be notified about the buffer getting full.
8- Synchronization is handled with binary semaphore.

