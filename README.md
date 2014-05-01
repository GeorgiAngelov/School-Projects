//////////////
DESCRIPTION
//////////////

This is a multi user chat system with a Server and a ChatClient. The Server is running on port 10841 and the ChatClients automatically connect to it. Furthermore, the Server is using the "users.txt" file which contains all username and password pairs for the created accounts.

//////////////
EXECUTION
//////////////

To run please first do the following:

1. Compile Server.java by executing: javac Server.java
2. Compile ChatClient.java by executing: javac ChatClient.java
3. Run Server by executing: java Server
4. Run ChatClient by executing: java ChatClient

//////////////
SPECIFICATIONS
//////////////

The chatroom contains certain functionalities that are described bellow.

1. login UserID Password 
If the server can verify the UserID and the Password, the server will send a 
confirmation message to the client and inform all other clients that this client joins the 
chat room; otherwise, the server will decline login and send an error message to the 
client.
2. newuser UserID Password 
Create a new user account. The length of the UserID should be less than 32, and the 
length of the Password should be between 4 and 8 characters. The server will reject 
the request if the UserID is already there. The users’ IDs and passwords should be 
kept in a file.
3. send all message
Send the “message” to the server. The server will precede the message with the 
UserID of the sender and broadcast the message to all other clients. 
4. send UserID message 
Send the “message” to the server. The server will precede the message with the 
UserID of the sender and unicast the message to the client “UserID”. 
5. who 
List all the clients in the chat room. 
6. logout 
Logout from the chat room. Once logout, the connection between the server and 
client will be closed. The server will inform all other clients that this client left
