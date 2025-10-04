/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 23:00:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/04 23:26:50 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(){}
Server::Server(const Server& right){
	this->port = right.port;
	this->serverSocket = right.serverSocket;
	this->pollManager = right.pollManager;
	this->serverAddress = right.serverAddress;
}

Server& Server::operator=(const Server& right){
	if (this != &right){
		this->port = right.port;
		this->serverSocket = right.serverSocket;
		this->pollManager = right.pollManager;
		this->serverAddress = right.serverAddress;
	}
	return (*this);
}

Server::Server(int port, std::string& password){
	if ((port < 0) || (port > MAX_PORTS))
		throw (Server::InvalidPortNumberException());
	else if (port < RESERVED_PORTS)
		throw (Server::ReservedPortException());
	if (password.empty())
		throw (Server::PasswordCannotBeEmptyException());
	this->port = port;
	this->password = std::string(password);
	/*
		AF_INET is just to specify that we are working with IPv4
		SOCK_STREAM provides 2 way communication.
		IPPROTO_TCP is the TCP protocol.

		@author Hamad
	*/
	this->serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (this->serverSocket < 0)
		throw (Server::FailedToInitServerSocketException());

	this->serverAddress.sin_family = AF_INET;
	this->serverAddress.sin_port = htons(this->port);
	this->serverAddress.sin_addr.s_addr = inet_addr(SERVER_IP.c_str());

	for (int i = 0; i < 8; i++)
		this->serverAddress.sin_zero[i] = 0;

	int bindResult = bind(
		this->serverSocket,
		(sockaddr *)&this->serverAddress,
		sizeof(this->serverAddress)
	);
	if (bindResult < 0){
		throw (Server::FailedToBindServerSocketException());
	}
	int listenResult = listen(this->serverSocket, NUMBER_OF_CLIENTS);
	if (listenResult < 0)
		throw (Server::FailedToListenException());
	try {
		this->clients = new pollfd[NUMBER_OF_CLIENTS];

	} catch (std::exception& err){
		(void)err;
		throw (Server::FailedToInitalizePollFd());
	}
	this->isRunning = true;
}

Server::~Server(){
	this->isRunning = false;
	this->port = -1;
	if (this->serverSocket >= 0){
		close(this->serverSocket);
		this->serverSocket = -1;
	}
	if (this->pollManager >= 0)
		this->pollManager = -1;
	this->serverAddress.sin_family = 0;
	this->serverAddress.sin_port = 0;
	this->serverAddress.sin_addr.s_addr = 0;
	for (size_t i = 0; i < 8; i++){
		this->serverAddress.sin_zero[i] = 0;
	}

	if (this->clients){
		delete[] (this->clients);
		this->clients = NULL;
	}
}

void	Server::start(void){
	while (this->isRunning){
		this->pollManager = poll(this->clients, NUMBER_OF_CLIENTS, MS_TIMEOUT);
		if (this->pollManager < 0){
			std::cerr << "\033[1;31mServer poll manager fail\033[0m" << std::endl;
			continue ;
		}
		else if (this->pollManager == 0){
			std::cerr << "\033[1;33mServer poll manager timed out\033[0m" << std::endl;
			continue ;
		}
			std::cout << "\033[1;32mServer poll manager operations start\033[0m" << std::endl;
			for (int i = 0; i < this->pollManager; i++){
				/*
				Here is where we are gonna check the client events. Anyways its
				11:26:35PM as im writing this imma head to sleep right now.
				*/
			}
	}
}


const char	*Server::InvalidPortNumberException::what() const throw(){
	return ("Invalid Port. Port must be between 0 and 65535");
}

const char	*Server::PasswordCannotBeEmptyException::what() const throw(){
	return ("The password cannot be empty");
}

const char	*Server::FailedToInitServerSocketException::what() const throw(){
	return ("Failed to initalize the server socket");
}

const char	*Server::FailedToBindServerSocketException::what() const throw(){
	return ("Failed to bind the socket to the port");
}

const char	*Server::FailedToInitPollException::what() const throw(){
	return ("Failed to initalize the poll");
}

const char	*Server::FailedToListenException::what() const throw(){
	return ("The server has failed to listen to sockets");
}

const char	*Server::ReservedPortException::what() const throw(){
	return ("Ports between 0 and 1023 are system reserved.");
}

const char	*Server::FailedToInitalizePollFd::what() const throw(){
	return ("The server failed to allocate memorey for pollfd.");
}
