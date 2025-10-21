/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 23:00:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/21 15:43:04 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server(){}
Server::Server(const Server& right){
	this->port = right.port;
	this->serverSocket = right.serverSocket;
	this->pollManager = right.pollManager;
	this->serverAddress = right.serverAddress;
	this->serverCapacity = right.serverCapacity;
}

Server& Server::operator=(const Server& right){
	if (this != &right){
		this->port = right.port;
		this->serverSocket = right.serverSocket;
		this->pollManager = right.pollManager;
		this->serverAddress = right.serverAddress;
		this->serverCapacity = right.serverCapacity;
	}
	return (*this);
}

Server::Server(int port, std::string& password){
	if ((port < 0) || (port > MAX_PORTS))
		throw (Server::InvalidPortNumberException());
	else if (port < RESERVED_PORTS)
		throw (Server::ReservedPortException());
	this->port = port;
	this->password = std::string(password);

	//I added 1 becuase the server will be inside pollfd as well.
	this->serverCapacity = NUMBER_OF_CLIENTS + 1;
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

	int listenResult = listen(this->serverSocket, this->serverCapacity);
	if (listenResult < 0)
		throw (Server::FailedToListenException());
	/*
		Added 0 at the end becuase it will tell the function when to stop the
		varadic function.
	*/	
	int fcntlResult = fcntl(this->serverSocket, F_SETFL, O_NONBLOCK);
	if (fcntlResult < 0)
		throw (Server::FailedToMakeTheSocketNonBlockingException());
	try {
		this->clients = new pollfd[this->serverCapacity];
		for (unsigned int i = 0; i < this->serverCapacity; i++){
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
	this->logFile.open("serverLog.log", std::ios::out | std::ios::trunc);
}

void	Server::closeClientConnection(pollfd& client){
	if (client.fd >= 0){
		close(client.fd);
	}
	client.fd = -1;
	client.events = 0;
	client.revents = 0;
}

Server::~Server(){
	logFile.close();
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
		for (unsigned int i = 0; i < this->serverCapacity; i++)
			closeClientConnection(this->clients[i]);
		delete[] (this->clients);
		this->clients = NULL;
	}
	this->serverCapacity = 0;
}

int	Server::checkHandshake(const std::string& handshake){
	if (handshake.substr(0, 4) == WEECHAT_PASS) {return (PASSWORD);}
	else if (handshake.substr(0, 6) == WEECHAT_CABAILITY_LS) {return (CABAILITY_LS);}
	else if (handshake.substr(0, 7) == WEECHAT_CABAILITY_REQ) {return (CABAILITY_REQ);}
	else if (handshake.substr(0, 7) == WEECHAT_CABAILITY_END) {return (CABAILITY_END);}
	else if (handshake.substr(0, 4) == WEECHAT_NICKNAME) {return (NICKNAME);}
	else if (handshake.substr(0, 4) == WEECHAT_USER) {return (USER);}
	return (-1);
}

std::string Server::constructHandshake(pollfd& client, int handshakeFlag){
	(void)client;
	std::ostringstream handshake;
	if (handshakeFlag & CABAILITY_LS) {
		handshake << ":";
		handshake << SERVER_NAME;
		handshake << " ";
		handshake << SERVER_CABAILITY;
	}
	else if (handshakeFlag & INVALID_PASSWORD) {
		handshake << ":";
		handshake << SERVER_NAME;
		handshake << " ";
		handshake << ERR_PASSWDMISMATCH;
		handshake << " * :Password incorrect\r\n";
	} else if (handshakeFlag & WELCOME_USER){
		handshake << ":";
		handshake << SERVER_NAME;
		handshake << " ";
		handshake << RPL_WELCOME;
		handshake << " :";
		handshake << CLIENT_CONNECTED;
		handshake << "\r\n";
	}
	return (handshake.str());
}

void	Server::performHandshake(pollfd& client, const std::string& handshake){
	int handshakeFlags = checkHandshake(handshake);
	Client& clientObj = this->clientMap.at(client.fd);
	std::string acknowledgementHandshake;

	if (handshakeFlags < 0){
			send(client.fd, CLIENT_SOMETHING_WENT_WRONG.c_str(), CLIENT_SOMETHING_WENT_WRONG.length(), DEFAULT_FLAG_SEND);
			this->clientMap.erase(client.fd);
			closeClientConnection(client);
			return ;
	}
	if (handshakeFlags & CABAILITY_END){
		handshakeFlags = WELCOME_USER;
		acknowledgementHandshake = constructHandshake(client, handshakeFlags);
		send(client.fd, acknowledgementHandshake.c_str(), acknowledgementHandshake.length(), MSG_DONTWAIT);
		
		return ;
	}
	if (handshakeFlags & CABAILITY_LS){
		acknowledgementHandshake = constructHandshake(client, handshakeFlags);
		if (acknowledgementHandshake.empty())
			return ;
		send(client.fd, acknowledgementHandshake.c_str(), acknowledgementHandshake.length(), MSG_DONTWAIT);
	}
	if (handshakeFlags & PASSWORD){
		std::string password = handshake.substr(5);
		password.erase(password.find_last_not_of("\r\n") + 1);
		if (password != this->password){
			handshakeFlags = INVALID_PASSWORD;
			acknowledgementHandshake = constructHandshake(client, handshakeFlags);
			send(client.fd, acknowledgementHandshake.c_str(), acknowledgementHandshake.length(), MSG_DONTWAIT);
			this->clientMap.erase(client.fd);
			closeClientConnection(client);
			return ;
		}
	} else if (handshakeFlags & NICKNAME){
		std::string nickname = handshake.substr(5);
		nickname.erase(nickname.find_last_not_of("\r\n") + 1);
		clientObj.setNickname(nickname);
	} else if (handshakeFlags & USER){
		size_t colonPos = handshake.find(':');
		std::string username = handshake.substr(colonPos + 1);
		username.erase(username.find_last_not_of("\r\n") + 1);
		username.erase(username.find_last_not_of("\r\n") + 1);
		clientObj.setUsername(username);
	}
}

std::string	Server::recieveData(pollfd& client){
	char	buffer[BUFFER_SIZE];
	ssize_t	recievedBytes = recv(client.fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (recievedBytes < 0)
		return (std::string(""));
	buffer[recievedBytes] = '\0';
	return (std::string(buffer));
}

void	Server::rejectClient(int clientSocket){
	close(clientSocket);
}

void	Server::start(void){
	while (this->isRunning){
		this->pollManager = poll(this->clients, NUMBER_OF_CLIENTS, MS_TIMEOUT);
		this->logFile.flush();
		if (this->pollManager <= 0){
			continue;
		}
		if (this->clients[0].revents & POLLIN){
			int clientSocket = accept(this->clients[0].fd, NULL, NULL);
			if (clientSocket < 0)
				continue ;
			if (this->clientMap.size() == serverCapacity){
				rejectClient(clientSocket);
				continue ;
			}
			for (unsigned int i = 1; i < this->serverCapacity; i++){
				pollfd&	client = this->clients[i];
				if (client.fd == -1){
					client.fd = clientSocket;
					client.events = POLLIN;
					this->clientMap[client.fd] = Client();
					this->clientBuffer[client.fd] = std::string("");
					break ;
				}
			}
		}
		for (unsigned int i = 1; i < this->serverCapacity; i++){
			pollfd&	client = this->clients[i];
			if ((client.fd >=0) && (client.revents & POLLIN)){
				std::string buffer = recieveData(client);
				if (buffer.empty())
					continue;
				std::string& clientBuffer = this->clientBuffer[client.fd];
				clientBuffer += buffer;
				size_t endPosition = clientBuffer.find("\r\n");
				while (endPosition != std::string::npos){
					std::string handshake = clientBuffer.substr(0, endPosition);
					performHandshake(client, handshake);
					this->logFile << clientBuffer;
					clientBuffer.erase(0, endPosition + 2);
					endPosition = clientBuffer.find("\r\n");
				}
			}
		}
	}
}

const char	*Server::InvalidPortNumberException::what() const throw(){
	return (INVALID_PORT.c_str());
}

const char	*Server::ReservedPortException::what() const throw(){
	return (RESERVED_PORT.c_str());
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
