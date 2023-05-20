# TCP-Client-Server-Chat-App

A chatting app implemented using C - Programming Language by using TCP-Socket programming. In this project multiple Client can send and receive message between each other until they are connected to the Server.
Clients can use multiple commands and command format provided by the server to communicate with the server and other actively connected clients via server.

## Requirements

    1. Linux OS (if you are windows user use WSL or any Virtual Machine with Linux installed)
    2. GCC compiler

If you consider using WSL here is how you can [install WSL](https://learn.microsoft.com/en-us/windows/wsl/install) and Ubuntu or any other distributor in your windows system

## Installation

Start by updating the package list

```bash
  sudo apt update
```

Install GCC compiler

```bash
  sudo apt-get install gcc -y
```

Check GCC version

```bash
  gcc --version
```

## Compile & Run the code

First compile Server.c and Client.c as server and client

```bash
  gcc -pthread server.c -o server
```

```bash
  gcc -pthread client.c -o client
```

Then run server

```bash
  ./server
```

Then run client in differnt terminal at same location (you can run same client to multiple terminal to connect more clients with the server)

```bash
  ./client
```

## Enjoy✌🏻
