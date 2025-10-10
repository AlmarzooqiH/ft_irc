/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 23:00:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/10 20:27:09 by hamalmar         ###   ########.fr       */
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

	int enable = 1;
	int setsockoptResult = setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
	if (setsockoptResult < 0)
		throw (Server::FailedToSetSocketOptionsException());
	int bindResult = bind(
		this->serverSocket,
		(sockaddr *)&this->serverAddress,
		sizeof(this->serverAddress)
	);
	if (bindResult < 0)
		throw (Server::FailedToBindServerSocketException());
std::cout << "serverSocket = " << serverSocket << std::endl;

	int listenResult = listen(this->serverSocket, NUMBER_OF_CLIENTS);
	if (listenResult < 0)
		throw (Server::FailedToListenException());
	/*
		Added 0 at the end becuase it will tell the function when to stop the
		varadic function.
	*/	
	int socketFlags = fcntl(this->serverSocket, F_GETFL, 0);
	int fcntlResult = fcntl(this->serverSocket, F_SETFL, socketFlags | O_NONBLOCK);
	if (fcntlResult < 0)
		throw (Server::FailedToMakeTheSocketNonBlockingException());
	try {
		this->clients = new pollfd[NUMBER_OF_CLIENTS];
		for (unsigned int i = 0; i < NUMBER_OF_CLIENTS; i++){
			this->clients[i].fd = -1;
			this->clients[i].events = 0;
			this->clients[i].revents = 0;
		}
		this->clients[0].fd = this->serverSocket;
		this->clients[0].events = POLLIN;
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
	std::ofstream logFile("server.log");
	std::ofstream errorLogFile("serverError.log");
	while (this->isRunning){
		this->pollManager = poll(this->clients, NUMBER_OF_CLIENTS, MS_TIMEOUT);
		if (this->pollManager < 0 ){
			errorLogFile << "Something went wrong" << std::endl;
			continue;
		} else if (this->pollManager == 0){
			errorLogFile << "Poll timeout" << std::endl;
			errorLogFile.flush();
			continue;
		}
		if (this->clients[0].revents & POLLIN){
			int clientSocket = accept(this->clients[0].fd, NULL, NULL);
			if (clientSocket < 0){
				errorLogFile << "Failed to accept client T-T </3" << std::endl;
				errorLogFile.flush();
				continue ;
			}
			for (int i = 1; i < NUMBER_OF_CLIENTS; i++){
				pollfd&	client = this->clients[i];
				if (client.fd == -1){
					client.fd = clientSocket;
					client.events = POLLIN;
					send(client.fd, CLIENT_CONNECTED.c_str(), CLIENT_CONNECTED.size(), MSG_DONTWAIT);
					logFile << "Client(" << client.fd << ") has successfully connect to HAI SERVER ^_^ <3" << std::endl;
					logFile.flush();
					break ;
				}
			}
		}
		for (int i = 1; i < NUMBER_OF_CLIENTS; i++){
			pollfd&	client = this->clients[i];
			if ((client.fd >=0) && (client.revents & POLLIN)){
				char	buffer[BUFFER_SIZE];
				ssize_t	recievedBytes = recv(client.fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
				if (recievedBytes < 0){
					errorLogFile << "Failed to recieve data from client(" << client.fd << ")" << std::endl;
					errorLogFile.flush();
					continue ;
				}
				buffer[recievedBytes] = '\0';
				logFile << "Recived the following message from client(" << client.fd << "): " << buffer << std::endl;
				logFile.flush();
			}
		}
	}
	logFile.close();
	errorLogFile.close();
}

const char	*Server::InvalidPortNumberException::what() const throw(){
	return (INVALID_PORT.c_str());
}

const char	*Server::ReservedPortException::what() const throw(){
	return (RESERVED_PORT.c_str());
}


const char	*Server::PasswordCannotBeEmptyException::what() const throw(){
	return (EMPTY_PASSWORD.c_str());
}

const char	*Server::FailedToInitServerSocketException::what() const throw(){
	return (SOCKET_INIT_FAIL.c_str());
}

const char	*Server::FailedToBindServerSocketException::what() const throw(){
	return (SOCKET_BIND_FAIL.c_str());
}

const char	*Server::FailedToListenException::what() const throw(){
	return (SOCKET_LISTEN_FAIL.c_str());
}

const char	*Server::FailedToInitalizePollFd::what() const throw(){
	return (POLLFD_INIT_FAIL.c_str());
}

const char	*Server::FailedToMakeTheSocketNonBlockingException::what() const throw(){
	return (SOCKET_NONBLOCKING_FAIL.c_str());
}

const char	*Server::FailedToSetSocketOptionsException::what() const throw() {
	return (SOCKET_OPTIONS_FAIL.c_str());
}
