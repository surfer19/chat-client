# Chat Client

## Description

Chat Client can be connected to server and wait to the input from terminal. User write message and after key 'enter' will message send to the server.
Server will automatically resend message to other clients connected on server.

## Installation

 1. clone project `git clone https://github.com/surfer19/chat-client.git`
 2. `cd chat-client/server`
 3. start server with `node app.js` (!IMPORTANT! to use command 'node' must be installed nodejs)
 4. open new terminal and go to project folder
 5. build client with `make`

## Usage

#### Start client:

    ./chat_client -i ip_address -u user_name

#### Example with our server:

    ./chat_client -i localhost -u Kamil

## Author

 Marián Mrva

## License

  Licensed under MIT.