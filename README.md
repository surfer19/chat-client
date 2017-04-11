# Chat Client

## Description

Chat Client can be connected to server and wait to the input from terminal. User write message and after key 'enter' will message send to the server.
Server will automatically resend message to other clients connected on server.

## Installation

 1. clone project `git clone https://github.com/surfer19/chat-client.git`
 2. `cd chat-client`
 3. build client with `make`
 4. run `chmod +x chat_client chat_server` for make executable files

## Usage

#### Start server:

start server at: `localhost` listen on port: `21011`

    ./chat_server

#### Start client:

    ./chat_client -i ip_address -u user_name

#### Example with our server:

    ./chat_client -i localhost -u Kamil

## Extras

     start another server in `chat_client/server` with `node app.js` (!IMPORTANT! to use command 'node' must be installed nodejs)

## Author

 Marián Mrva

## License

  Licensed under MIT.