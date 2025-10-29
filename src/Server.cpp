/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 23:00:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/12 01:28:39 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Message.hpp"

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

int	Server::checkHandshake(const std::string& handshake){
	if (handshake.substr(0, 4) == WEECHAT_PASS) {return (PASSWORD);}
	else if (handshake.substr(0, 6) == WEECHAT_CABAILITY_LS) {return (CABAILITY_LS);}
	else if (handshake.substr(0, 7) == WEECHAT_CABAILITY_REQ) {return (CABAILITY_REQ);}
	else if (handshake.substr(0, 7) == WEECHAT_CABAILITY_END) {return (CABAILITY_END);}
	else if (handshake.substr(0, 4) == WEECHAT_NICKNAME) {return (NICKNAME);}
	else if (handshake.substr(0, 4) == WEECHAT_USER) {return (USER);}
	return (-1);
}

std::string Server::constructHandshake(int handshakeFlag){
	std::string handshake("");
	if (handshakeFlag & CABAILITY_LS) {
		handshake += ":";
		handshake += SERVER_NAME;
		handshake += " ";
		handshake += SERVER_CABAILITY;
	}
	else if (handshakeFlag & INVALID_PASSWORD) {
		handshake += ":";
		handshake += SERVER_NAME;
		handshake += " ";
		handshake += "464 * :Password incorrect\r\n";
	}
	return (handshake);
}

void	Server::performHandshake(pollfd& client, const std::string& handshake){
	int handshakeFlags = checkHandshake(handshake);
	std::string acknowledgementHandshake;

	if (handshakeFlags < 0){
			send(client.fd, CLIENT_SOMETHING_WENT_WRONG.c_str(), CLIENT_SOMETHING_WENT_WRONG.length(), MSG_DONTWAIT);
			this->clientMap.erase(client.fd);
			close(client.fd);
			client.fd = -1;
			return ;
	}
	if (handshakeFlags & CABAILITY_END)
		return ;
	if (handshakeFlags & CABAILITY_LS){
		acknowledgementHandshake = constructHandshake(handshakeFlags);
		if (acknowledgementHandshake.empty())
			return ;
		send(client.fd, acknowledgementHandshake.c_str(), acknowledgementHandshake.length(), MSG_DONTWAIT);
	}
	if (handshakeFlags & PASSWORD){
		std::string password = handshake.substr(5);
		password.erase(password.find_last_not_of("\r\n") + 1);
		if (password != this->password){
			handshakeFlags = INVALID_PASSWORD;
			acknowledgementHandshake = constructHandshake(handshakeFlags);
			if (!acknowledgementHandshake.empty())
				send(client.fd, acknowledgementHandshake.c_str(), acknowledgementHandshake.length(), MSG_DONTWAIT);
			this->clientMap.erase(client.fd);
			close(client.fd);
			client.fd = -1;
			return ;
		}
	} else if (handshakeFlags & NICKNAME){
		std::string nickname = handshake.substr(5);
		nickname.erase(nickname.find_last_not_of("\r\n") + 1);
		this->clientMap.at(client.fd).setNickname(nickname);
	}
}

/**
 * @brief Handle a complete IRC message using proper RFC 2812 parsing
 * 
 * This method replaces the simple string-based handshake parsing with
 * proper message parsing according to IRC protocol specification.
 * 
 * @param client The client pollfd structure
 * @param rawMessage The raw message string (without \r\n)
 */
void	Server::handleMessage(pollfd& client, const std::string& rawMessage){
	Message msg(rawMessage);
	
	if (!msg.isValid()){
		send(client.fd, CLIENT_SOMETHING_WENT_WRONG.c_str(), CLIENT_SOMETHING_WENT_WRONG.length(), MSG_DONTWAIT);
		return;
	}

	std::string command = msg.getCommand();
	std::vector<std::string> params = msg.getParameters();

	processCommand(client.fd, command, params);
}

/**
 * @brief Process IRC commands according to RFC 2812
 * 
 * @param clientFd The client file descriptor
 * @param command The IRC command (e.g., PASS, NICK, USER, JOIN, PRIVMSG)
 * @param params The command parameters
 */
void	Server::processCommand(int clientFd, const std::string& command, const std::vector<std::string>& params){
	std::string cmd = command;
	for (size_t i = 0; i < cmd.length(); i++) {
		cmd[i] = std::toupper(cmd[i]); //Might be c++11 not sure yet
	}

	if (cmd == "CAP") {
		if (params.size() > 0) {
			std::string subCmd = params[0];
			for (size_t i = 0; i < subCmd.length(); i++) {
				subCmd[i] = std::toupper(subCmd[i]);
			}
			
			if (subCmd == "LS") {
				std::string response = constructHandshake(CABAILITY_LS);
				send(clientFd, response.c_str(), response.length(), MSG_DONTWAIT);
			}
		}
		return;
	}

	if (cmd == "PASS") {
		if (params.size() < 1) {
			std::string error = ":HAI 461 * PASS :Not enough parameters\r\n";
			send(clientFd, error.c_str(), error.length(), MSG_DONTWAIT);
			return;
		}
		
		std::string password = params[0];
		if (password != this->password) {
			std::string response = constructHandshake(INVALID_PASSWORD);
			send(clientFd, response.c_str(), response.length(), MSG_DONTWAIT);
			this->clientMap.erase(clientFd);
			close(clientFd);
			return;
		}
		return;
	}

	if (cmd == "NICK") {
		if (params.size() < 1) {
			std::string error = ":HAI 431 * :No nickname given\r\n";
			send(clientFd, error.c_str(), error.length(), MSG_DONTWAIT);
			return;
		}
		
		std::string nickname = params[0];
		// TODO: Validate nickname format and check for duplicates
		this->clientMap[clientFd].setNickname(nickname);
		return;
	}

	if (cmd == "USER") {
		if (params.size() < 4) {
			std::string error = ":HAI 461 * USER :Not enough parameters\r\n";
			send(clientFd, error.c_str(), error.length(), MSG_DONTWAIT);
			return;
		}
		
		std::string username = params[0];
		// params[1] is mode (usually "0")
		// params[2] is unused (usually "*")
		// params[3] is realname
		this->clientMap[clientFd].setUsername(username);
		return;
	}

	// Add more command handlers here (JOIN, PART, PRIVMSG, etc.)
	
	std::string error = ":HAI 421 * " + command + " :Unknown command\r\n";
	send(clientFd, error.c_str(), error.length(), MSG_DONTWAIT);
}

std::string	Server::recieveData(pollfd& client){
	char	buffer[BUFFER_SIZE];
	ssize_t	recievedBytes = recv(client.fd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	if (recievedBytes < 0)
		return (std::string(""));
	buffer[recievedBytes] = '\0';
	return (std::string(buffer));
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
			continue;
		}
		if (this->clients[0].revents & POLLIN){
			int clientSocket = accept(this->clients[0].fd, NULL, NULL);
			if (clientSocket < 0){
				errorLogFile << "Failed to accept client T-T </3" << std::endl;
				continue ;
			}
			for (int i = 1; i < NUMBER_OF_CLIENTS; i++){
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
		for (int i = 1; i < NUMBER_OF_CLIENTS; i++){
			pollfd&	client = this->clients[i];
			if ((client.fd >=0) && (client.revents & POLLIN)){
				std::string buffer = recieveData(client);
				if (buffer.empty()){
					errorLogFile << "Failed to recieve data from client(" << client.fd << ")" << std::endl;
					continue;
				}
			std::string& clientBuffer = this->clientBuffer[client.fd];
			clientBuffer += buffer;
			size_t endPosition = clientBuffer.find("\r\n");
			while (endPosition != std::string::npos){
				std::string message = clientBuffer.substr(0, endPosition);
				
				// Use the new RFC 2812 compliant message handler
				handleMessage(client, message);
				// performHandshake(client, message);

				clientBuffer.erase(0, endPosition + 2);
				endPosition = clientBuffer.find("\r\n");
			}
				logFile << buffer << std::endl;
			}
		}
		logFile.flush();
		errorLogFile.flush();
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
