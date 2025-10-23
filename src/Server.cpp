/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 23:00:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/23 13:31:57 by hamalmar         ###   ########.fr       */
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

Server::Server(int port, const std::string& password){
	if ((port < 0) || (port > MAX_PORTS))
		throw (Server::InvalidPortNumberException());
	else if (port < RESERVED_PORTS)
		throw (Server::ReservedPortException());
	this->port = port;
	this->password = password;

	//I added 1 becuase the server will be inside pollfd as well.
	this->serverCapacity = NUMBER_OF_CLIENTS + 1;
	/**
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
}

/**
 * @brief This function will close the client connection and set the pollfd
 * to available.
 * @param client The client.
 * @return void.
 * @author Hamad
 */
void	Server::closeClientConnection(pollfd& client){
	if (client.fd >= 0){
		close(client.fd);
	}
	client.fd = -1;
	client.events = 0;
	client.revents = 0;
}

/**
 * @brief Here is where we will do the Server clean up.
 * @author Hamad
 */
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
		for (unsigned int i = 0; i < this->serverCapacity; i++)
			closeClientConnection(this->clients[i]);
		delete[] (this->clients);
		this->clients = NULL;
	}
	this->serverCapacity = 0;
}

/**
 * @brief This function will check what kind of operation did weechat send.
 * @param The handshake we recieved.
 * @return If successful it will return a bitmasked value
 * according to the requested operation.
 * 
 * @return On failure it will return -1.
 * @author Hamad
 */
int	Server::checkHandshake(const std::string& handshake){
	if (handshake.substr(0, 4) == WEECHAT_PASS) {return (PASSWORD);}
	else if (handshake.substr(0, 6) == WEECHAT_CABAILITY_LS) {return (CABAILITY_LS);}
	else if (handshake.substr(0, 7) == WEECHAT_CABAILITY_REQ) {return (CABAILITY_REQ);}
	else if (handshake.substr(0, 7) == WEECHAT_CABAILITY_END) {return (CABAILITY_END);}
	else if (handshake.substr(0, 4) == WEECHAT_NICKNAME) {return (NICKNAME);}
	else if (handshake.substr(0, 4) == WEECHAT_USER) {return (USER);}
	else if (handshake.substr(0, 4) == WEECHAT_QUIT) {return (QUIT);}
	else if (handshake.substr(0, 4) == WEECHAT_LIST) {return (LIST);}
	else if (handshake.substr(0, 4) == WEECHAT_PING) {return (PING);}
	return (-1);
}

/**
 * @brief This function will build the server reply that will be sent back to the
 * client.
 * 
 * @param client The client. Will be used to get his nickname.
 * @param handshakeFlag The bitmasked value that we got from checkHandshake() and
 * will construct an apropirate message according the handshake.
 * 
 * @return The appropirate reply to the client and will be used in sendMessage().
 * @author Hamad
 */
std::string Server::constructHandshake(pollfd& client, int handshakeFlag){
	Client& clientObj = this->clientMap.at(client.fd);
	std::ostringstream handshake;
	if (handshakeFlag & CABAILITY_LS) {
		handshake << ":" << SERVER_NAME << " ";
		handshake << "CAP " << clientObj.getNickname() << " LS :";
		handshake << CLDR;
	} else if (handshakeFlag & INVALID_PASSWORD) {
		handshake << ":" << SERVER_NAME << " " << ERR_PASSWDMISMATCH << " * :";
		handshake << MSG_INVALID_PASSWORD << CLDR;
	} else if (handshakeFlag & WELCOME_USER) {
		std::ostringstream num;
		handshake << ":" << SERVER_NAME << " "
		<< std::setw(3) << std::setfill('0') << RPL_WELCOME
		<< " " << clientObj.getNickname() << " :" << MSG_WELCOME << CLDR;
		
		handshake << ":" << SERVER_NAME << " "
		<< std::setw(3) << std::setfill('0') << RPL_YOURHOST
		<< " " << clientObj.getNickname() << " :Your host is " << SERVER_NAME
		<< ", running version " << SERVER_VERSION << CLDR;

		handshake << ":" << SERVER_NAME << " "
		<< std::setw(3) << std::setfill('0') << RPL_CREATED
		<< " " << clientObj.getNickname() << " :" << MSG_SERVER_CREATION << CLDR;
		
		handshake << ":" << SERVER_NAME << " "
		<< std::setw(3) << std::setfill('0') << RPL_MYINFO
		<< " " << clientObj.getNickname() << " :" << SERVER_NAME << " "
		<< SERVER_VERSION << " iowghra" << CLDR;
	} else if (handshakeFlag & NICKNAME_TAKEN){
		handshake << ":" << SERVER_NAME << " " << ERR_NICKNAMEINUSE
		<< " * " << clientObj.getNickname() << " :" << MSG_NICKNAME_TAKEN << CLDR;
	} else if (handshakeFlag & PING){
		//we will reply to client with PONG
		handshake << WEECHAT_PONG << " :" << SERVER_NAME << CLDR;
	}
	return (handshake.str());
}

/**
 * @brief This function will gurantee the message to be sent.
 * @param client The client.
 * @param message The message that we want to send.
 * @note If send() fails we will break from the loop.
 * @return void.
 */
void	Server::sendMessage(pollfd& client, std::string& message){
	size_t totalBytesSent = 0;
	size_t	messageLength = message.length();

	while (totalBytesSent < messageLength){
		size_t sentBytes = send(client.fd, message.c_str() + totalBytesSent, messageLength - totalBytesSent, DEFAULT_FLAG_SEND);
		if (sentBytes <= 0)
			break;
		totalBytesSent += sentBytes;
	}
	std::cout << "Server sent: " << message;
}

bool	Server::isNicknameTaken(std::string& nickname){
	for (size_t i = 1; i < this->serverCapacity; i++){
		pollfd &client = this->clients[i];
		if (client.fd < 0)
			continue;
		Client &clientObj = this->clientMap[client.fd];
		if (clientObj.getNickname() == nickname)
			return (true);
	}
	return (false);
}

/**
 * @brief This function will check what kind of operation we recived from the client
 * and will send back to the client the message appropiratley.
 * 
 * @param client The client that sent the message.
 * @param handshake The client message that we want to process.
 * 
 * @return void.
 * @author Hamad
 */
void	Server::performHandshake(pollfd& client, const std::string& handshake){
	std::cout << "Recived handshake: " << handshake << std::endl;
	int handshakeFlags = checkHandshake(handshake);
	Client& clientObj = this->clientMap.at(client.fd);
	std::string acknowledgementHandshake;

	if (handshakeFlags < 0)
		return ;
	if (!(this->password.empty()) && (handshakeFlags & PASSWORD)){
		std::string password = handshake.substr(5);
		password.erase(password.find_last_not_of("\r\n") + 1);
		if (password != this->password){
			handshakeFlags = INVALID_PASSWORD;
			acknowledgementHandshake = constructHandshake(client, handshakeFlags);
			sendMessage(client, acknowledgementHandshake);
			closeClientConnection(client);
			return ;
		}
	}
	if (handshakeFlags & CABAILITY_LS)
		clientObj.setRecvCab(true);
	if (handshakeFlags & NICKNAME){
		std::string nickname = handshake.substr(5);
		nickname.erase(nickname.find_last_not_of("\r\n") + 1);
		std::cout << "Is nickname taken: " << isNicknameTaken(nickname) << std::endl;
		if (!isNicknameTaken(nickname)){
			clientObj.setNickname(nickname);
		}else{
			handshakeFlags = NICKNAME_TAKEN;
			std::string tmpNickname = clientObj.getNickname();
			clientObj.setNickname(nickname);
			acknowledgementHandshake = constructHandshake(client, handshakeFlags);
			clientObj.setNickname(tmpNickname);
			return ;
		}
		clientObj.setRecvNick(true);
		std::cout << "Client nickname: " << clientObj.getNickname() << std::endl;
	}
	if (handshakeFlags & USER){
		std::string username = handshake.substr(handshake.find(" ") + 1);
		username.erase(username.find(" "), username.length());
		clientObj.setUsername(username);
		clientObj.setRecvUser(true);
	}

	if (handshakeFlags & CABAILITY_END){
		handshakeFlags = WELCOME_USER;
		acknowledgementHandshake = constructHandshake(client, handshakeFlags);
		clientObj.setRole(CLIENT_ROLE_REGULAR);
	}

	if (handshakeFlags & QUIT){
		closeClientConnection(client);
		return ;
	}

	if (handshakeFlags & LIST)
		acknowledgementHandshake = constructHandshake(client, handshakeFlags);

	if (handshakeFlags & PING)
		acknowledgementHandshake = constructHandshake(client, handshakeFlags);

	if (!acknowledgementHandshake.empty())
		sendMessage(client, acknowledgementHandshake);
	
	if (clientObj.getRecvNick() && clientObj.getRecvCab()){
		handshakeFlags = CABAILITY_LS;
		acknowledgementHandshake = constructHandshake(client, handshakeFlags);
		sendMessage(client, acknowledgementHandshake);
		clientObj.setRecvCab(false);
		std::cout << "Sent to client cabaiblites: " << acknowledgementHandshake;
	}
}

/**
 * @brief This function will recieve the data from the client via recv().
 * 
 * @param client The client that wants to send data.
 * @return The message that the client has sent.
 * @author Hamad
 */
std::string	Server::recieveData(pollfd& client){
	char	buffer[BUFFER_SIZE];
	ssize_t	recievedBytes = recv(client.fd, buffer, BUFFER_SIZE, DEFAULT_FLAG_SEND);
	if (recievedBytes < 0)
		return (std::string(""));
	buffer[recievedBytes] = '\0';
	return (std::string(buffer));
}

/**
 * @brief This function will reject the client.
 * 
 * @param clientSocket The client.
 * @note This function will be called only when the server is full. This is why
 * i'm not using pollfd.
 * 
 * @return void.
 * @author Hamad
 */
void	Server::rejectClient(int clientSocket){
	close(clientSocket);
}

/**
 * @brief This function is responsible to accept/reject clients. It will also handel
 * client messages or commands via performHandshake().
 * 
 * @return void.
 * @author Hamad
 */
void	Server::start(void){
	while (this->isRunning){
		this->pollManager = poll(this->clients, this->serverCapacity, MS_TIMEOUT);
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
					clientBuffer.erase(0, endPosition + 2);
					endPosition = clientBuffer.find("\r\n");
				}
			}
		}
	}
}

/*
			 ____  _____ ______     _______ ____  
			/ ___|| ____|  _ \ \   / / ____|  _ \ 
			\___ \|  _| | |_) \ \ / /|  _| | |_) |
			 ___) | |___|  _ < \ V / | |___|  _ < 
			|____/|_____|_| \_\ \_/  |_____|_| \_\

			 _______  ______ _____ ____ _____ ___ ___  _   _ ____  
			| ____\ \/ / ___| ____|  _ \_   _|_ _/ _ \| \ | / ___| 
			|  _|  \  / |   |  _| | |_) || |  | | | | |  \| \___ \ 
			| |___ /  \ |___| |___|  __/ | |  | | |_| | |\  |___) |
			|_____/_/\_\____|_____|_|    |_| |___\___/|_| \_|____/ 
*/

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
