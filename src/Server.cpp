/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 23:00:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/11/25 00:53:57 by hamalmar         ###   ########.fr       */
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
	channels.insert(std::make_pair("#general", Channel("#general")));
	channels.insert(std::make_pair("#random", Channel("#random")));
	channels.insert(std::make_pair("#help", Channel("#help")));
	channels.insert(std::make_pair("#admins", Channel("#admins")));
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
 * @brief Send welcome messages after successful registration (001-004)
 * @param clientFd The client file descriptor
 */
void Server::sendWelcomeMessages(pollfd& client) {
    Client& clientObj = this->clientMap[client.fd];
    std::string nick = clientObj.getNickname();
    std::string user = clientObj.getUsername();
    std::ostringstream ss;

    // 001 RPL_WELCOME
    ss.str(""); ss.clear();
    ss << ":" << SERVER_NAME << " " << RPL_WELCOME << " " << nick
       << " :" << MSG_WELCOME << " " << nick << "!" << user << "@localhost" << CLDR;
    sendMessage(client, ss.str());

    // 002 RPL_YOURHOST
    ss.str(""); ss.clear();
    ss << ":" << SERVER_NAME << " " << RPL_YOURHOST << " " << nick
       << " :Your host is " << SERVER_NAME << ", running version " << SERVER_VERSION << CLDR;
    sendMessage(client, ss.str());

    // 003 RPL_CREATED
    ss.str(""); ss.clear();
    ss << ":" << SERVER_NAME << " " << RPL_CREATED << " " << nick
       << " :" << MSG_SERVER_CREATION << CLDR;
    sendMessage(client, ss.str());

    // 004 RPL_MYINFO
    ss.str(""); ss.clear();
    ss << ":" << SERVER_NAME << " " << RPL_MYINFO << " " << nick
       << " " << SERVER_NAME << " " << SERVER_VERSION << " o o" << CLDR;
    sendMessage(client, ss.str());
}

/**
 * @brief Validate nickname format according to RFC 2812
 * Nickname = ( letter / special ) *8( letter / digit / special / "-" )
 * special = %x5B-60 / %x7B-7D  ; "[", "]", "\", "`", "_", "^", "{", "|", "}"
 * @param nickname The nickname to validate
 * @return true if valid, false otherwise
 */
bool	Server::isNicknameValid(const std::string& nickname){
	if (nickname.empty() || nickname.length() > 9) {
		return false;
	}
	
	// First character must be letter or special character
	char first = nickname[0];
	bool validFirst = (first >= 'A' && first <= 'Z') || 
	                  (first >= 'a' && first <= 'z') ||
	                  (first >= '[' && first <= '`') ||
	                  (first >= '{' && first <= '}');
	
	if (!validFirst) {
		return false;
	}
	
	// Rest can be letter, digit, special, or dash
	for (size_t i = 1; i < nickname.length(); i++) {
		char c = nickname[i];
		bool valid = (c >= 'A' && c <= 'Z') || 
		             (c >= 'a' && c <= 'z') ||
		             (c >= '0' && c <= '9') ||
		             (c >= '[' && c <= '`') ||
		             (c >= '{' && c <= '}') ||
		             (c == '-');
		
		if (!valid) {
			return false;
		}
	}
	
	return true;
}

/**
 * @brief Check if a nickname is already in use by another client
 * @param nickname The nickname to check
 * @return true if in use, false otherwise
 */
bool	Server::isNicknameInUse(const std::string& nickname){
	for (std::map<int, Client>::iterator it = this->clientMap.begin(); 
	     it != this->clientMap.end(); ++it) {
		if (it->second.getNickname() == nickname) {
			return true;
		}
	}
	return false;
}

/**
 * @brief Send a numeric reply to a client (RFC 2812 format)
 * @param clientFd The client file descriptor
 * @param numeric The numeric reply code (e.g., "001", "461")
 * @param message The message text
 */
void	Server::sendNumericReply(pollfd& client, const std::string& numeric, const std::string& message){
	std::string nickname = "*";
	
	// Use actual nickname if client has set one
	if (this->clientMap.find(client.fd) != this->clientMap.end()) {
		Client& clientObj = this->clientMap[client.fd];
		if (clientObj.isNicknameSet()) {
			nickname = clientObj.getNickname();
		}
	}
	
	std::string response = ":" + SERVER_NAME + " " + numeric + " " + nickname + " " + message + CLDR;
	sendMessage(client, response);
}

void Server::cleanClient(pollfd& client) {
    if (client.fd >= 0) {
		std::cout << this->clientMap[client.fd].getNickname() << " Has disconnected!" << std::endl;
        clientMap.erase(client.fd);
        clientBuffer.erase(client.fd);
        close(client.fd);
        client.fd = -1;
    }
}

/**
 * @brief Process IRC commands according to RFC 2812
 * 
 * @param clientFd The client file descriptor
 * @param command The IRC command (e.g., PASS, NICK, USER, JOIN, PRIVMSG)
 * @param params The command parameters
 */
void	Server::processCommand(pollfd& client, const std::string& command, const std::vector<std::string>& params){
	std::string cmd = command;
	for (size_t i = 0; i < cmd.length(); i++) {
		cmd[i] = std::toupper(cmd[i]);
	}

	Client& clientObj = this->clientMap[client.fd];

	// Handle CAP command (client capability negotiation)
	if (cmd == "CAP") {
		if (params.size() > 0) {
			std::string subCmd = params[0];
			for (size_t i = 0; i < subCmd.length(); i++) {
				subCmd[i] = std::toupper(subCmd[i]);
			}
			
			if (subCmd == "LS") {
				std::string response = ":" + SERVER_NAME + " CAP " + clientObj.getNickname() + " LS :" + CLDR;
				sendMessage(client, response);
			}
		}
		return;
	}

	// Handle PASS command - must be first command before registration
	if (cmd == "PASS") {
		if (clientObj.isFullyRegistered()) {
			sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
			return;
		}
		
		if (params.size() < 1) {
			sendNumericReply(client, ERR_NEEDMOREPARAMS, "PASS :Not enough parameters");
			return;
		}
		
		std::string password = params[0];
		if (password != this->password) {
			sendNumericReply(client, ERR_PASSWDMISMATCH, ":Password incorrect");
			this->clientMap.erase(client.fd);
			close(client.fd);
			return;
		}
		
		clientObj.setPasswordAuthenticated(true);
		return;
	}

	// Handle NICK command - set or change nickname
	if (cmd == "NICK") {
		if (params.size() < 1) {
			sendNumericReply(client, ERR_NONICKNAMEGIVEN, ":No nickname given");
			return;
		}
		
		std::string nickname = params[0];
		
		if (!isNicknameValid(nickname)) {
			sendNumericReply(client, ERR_ERRONEUSNICKNAME, nickname + " :Erroneous nickname");
			return;
		}
		
		if (isNicknameInUse(nickname)) {
			sendNumericReply(client, ERR_NICKNAMEINUSE, nickname + " :Nickname is already in use");
			return;
		}

		bool wasRegistered = clientObj.isFullyRegistered();
		clientObj.setNickname(nickname);
		clientObj.setNicknameSet(true);
		
		if (!wasRegistered && clientObj.isFullyRegistered()) {
			sendWelcomeMessages(client);
		}
		return;
	}

	/*USER john 0 * :John Doe
     ^^^^ ^ ^  ^^^^^^^^^
            |    | |  └─ Real name (can have spaces)
            |    | └──── Unused (always *)
            |    └────── Mode (usually 0)
            └─────────── Username

	USER alice 0 * :Alice from Wonderland
	USER bot123 0 * :IRC Bot v1.0
	USER testuser 0 * :Test Account
	USER admin 8 * :Server Administrator  ← 8 = invisible mode (rarely used)*/
	
	if (cmd == "USER") {
		if (clientObj.isUserSet()) {
			sendNumericReply(client, ERR_ALREADYREGISTRED, ":You may not reregister");
			return;
		}
		
		if (params.size() < 4) {
			sendNumericReply(client, ERR_NEEDMOREPARAMS, "USER :Not enough parameters");
			return;
		}
		
		std::string username = params[0];
		std::string realname = params[3];
		
		bool wasRegistered = clientObj.isFullyRegistered();
		clientObj.setUsername(username);
		clientObj.setRealname(realname);
		clientObj.setUserSet(true);
		
		if (!wasRegistered && clientObj.isFullyRegistered()) {
			sendWelcomeMessages(client);
		}
		return;
	}

	// Commands that require authentication
	if (!clientObj.isPasswordAuthenticated()) {
		sendNumericReply(client, ERR_NOTREGISTERED, ":You have not registered");
		return;
	}

		if (cmd == WEECHAT_PING){
			std::string response = WEECHAT_PONG + " :" + SERVER_NAME + CLDR;
			sendMessage(client, response);
			return ;
		}
		if (cmd == WEECHAT_QUIT){
			cleanClient(client);
			return;
		}
		if (cmd == WEECHAT_LIST) {
			for (std::map<std::string, Channel>::iterator it = channels.begin(); it != channels.end(); ++it) {
				Channel &chan = it->second;
				std::ostringstream oss;
				oss << chan.getMemberCount();
			std::string memberCountStr = oss.str();
			std::string msg = ":" + SERVER_NAME + " " + RPL_LIST + " " +
				clientObj.getNickname() + " " +
				chan.getName() + " " +
				memberCountStr +
				" :" + chan.getTopic() + CLDR;
			sendMessage(client, msg); 
		}
		std::string endMsg = ":" + SERVER_NAME + " " + RPL_LISTEND + " " +
		clientObj.getNickname() + " :End of /LIST" + CLDR;
		sendMessage(client, endMsg);
		return;
	}
	if (cmd == WEECHAT_JOIN) {
		if (params.size() < 1) {
			sendNumericReply(client, ERR_NEEDMOREPARAMS, "JOIN :Not enough parameters");
			return;
		}
		std::string channelName = params[0];

		// Check if the channel exists
		std::map<std::string, Channel>::iterator it = channels.find(channelName);
		if (it == channels.end()) {
			sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
			return;
		}
		Channel &chan = it->second;

		// If invite-only, make sure the client is invited
		if (chan.isInviteOnly() && !chan.isInvited(client.fd)) {
			sendNumericReply(client, ERR_INVITEONLYCHAN, channelName + " :Cannot join channel (+i)");
			return;
		}

		// If channel has a key (+k), check if provided
		if (!chan.getKey().empty()) {
			if (params.size() < 2 || params[1] != chan.getKey()) {
				sendNumericReply(client, ERR_BADCHANNELKEY, channelName + " :Cannot join channel (+k)");
				return;
			}
		}

		// If user limit (+l) is set
		if (chan.getUserLimit() > 0 && (int)chan.getMemberCount() >= chan.getUserLimit()) {
			sendNumericReply(client, ERR_CHANNELISFULL, channelName + " :Cannot join channel (+l)");
			return;
		}

		// Add client to channel
		chan.addMember(client.fd);

		// Broadcast JOIN to all channel members
		std::string joinMsg = ":" + clientObj.getNickname() + " JOIN " + channelName + CLDR;
		chan.broadcast(joinMsg);

		// Send topic if any
		std::string topic = chan.getTopic();
		if (!topic.empty()) {
			sendNumericReply(client, RPL_TOPIC, channelName + " :" + topic);
		}

		// Send NAMES list
		std::string namesReply = chan.getNamesReply(clientMap);
		sendMessage(client, namesReply);
		return;
	}
	if (cmd == WEECHAT_PRIVMSG){
		// Need at least target + message
		if (params.size() < 2){
			sendNumericReply(client, ERR_NEEDMOREPARAMS, "PRIVMSG :Not enough parameters");
			return;
		}
		std::string channelName = params[0];

		// Channel must exist
		if (this->channels.find(channelName) == this->channels.end()){
			sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
			return;
		}

		// Get channel by reference
		Channel &channel = this->channels[channelName];

		// Sender must be in the channel
		if (!channel.hasMember(client.fd)){
			sendNumericReply(client, ERR_CANNOTSENDTOCHAN, channelName + " :Cannot send to channel");
			return;
		}

		// Build the message (no host, as requested)
		std::string fullMsg;
		fullMsg  = ":" + clientObj.getNickname();
		fullMsg += "!" + clientObj.getUsername();
		fullMsg += " PRIVMSG " + channelName + " :" + params[1] + CLDR;

		// Broadcast to everyone except sender
		channel.broadcast(fullMsg, client.fd);
		return;
	}

	// Unknown command
	sendNumericReply(client, ERR_UNKNOWNCOMMAND, command + " :Unknown command");
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
		sendMessage(client, MSG_SOMETHING_WENT_WRONG);
		return;
	}

	std::string command = msg.getCommand();
	std::vector<std::string> params = msg.getParameters();
	processCommand(client, command, params);
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
			pollfd& client = this->clients[i];
			if ((client.fd >= 0) && (client.revents & POLLIN)){
				std::string buffer = recieveData(client);
				if (buffer.empty())
					continue;
				std::string& clientBuffer = this->clientBuffer[client.fd];
				clientBuffer += buffer;
				size_t endPosition = clientBuffer.find(CLDR);
				while (endPosition != std::string::npos){
					std::string message = clientBuffer.substr(0, endPosition);
					// Use RFC-compliant message handler
					handleMessage(client, message);
					clientBuffer.erase(0, endPosition + 2);
					endPosition = clientBuffer.find(CLDR);
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

const char* Server::InvalidPortNumberException::what() const throw() {
    return INVALID_PORT.c_str();
}

const char* Server::ReservedPortException::what() const throw() {
    return RESERVED_PORT.c_str();
}

const char* Server::FailedToInitServerSocketException::what() const throw() {
    return SOCKET_INIT_FAIL.c_str();
}

const char* Server::FailedToBindServerSocketException::what() const throw() {
    return SOCKET_BIND_FAIL.c_str();
}

const char* Server::FailedToListenException::what() const throw() {
    return SOCKET_LISTEN_FAIL.c_str();
}

const char* Server::FailedToInitalizePollFd::what() const throw() {
    return POLLFD_INIT_FAIL.c_str();
}

const char* Server::FailedToMakeTheSocketNonBlockingException::what() const throw() {
    return SOCKET_NONBLOCKING_FAIL.c_str();
}

const char* Server::FailedToSetSocketOptionsException::what() const throw() {
    return SOCKET_OPTIONS_FAIL.c_str();
}

