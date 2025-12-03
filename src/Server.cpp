/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ialashqa <ialashqa@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/01 23:00:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/11/26 17:02:17 by ialashqa         ###   ########.fr       */
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
    if (client.fd < 0)
        return;
    
    // Check if client exists in map
    std::map<int, Client>::iterator clientIt = clientMap.find(client.fd);
    if (clientIt == clientMap.end()) {
        // Client not in map, just close the fd
        close(client.fd);
        client.fd = -1;
        client.events = 0;
        client.revents = 0;
        return;
    }
    
    std::string nickname = clientIt->second.getNickname();
    if (nickname.empty())
        nickname = "*";
    
    std::cout << nickname << " Has disconnected!" << std::endl;
    
    // Remove from all channels and handle auto-promotion
    for (std::map<std::string, Channel>::iterator it = channels.begin(); 
         it != channels.end(); ++it) {
        Channel& chan = it->second;
        if (chan.hasMember(client.fd)) {
            // Broadcast QUIT to channel members (before removing)
            std::string quitMsg = ":" + nickname + " QUIT :Client disconnected" + CLDR;
            chan.broadcast(quitMsg, client.fd);
            
            // Remove and check for auto-promotion
            int newOpFd = chan.removeMember(client.fd);
            
            // If someone was auto-promoted, broadcast MODE +o
            if (newOpFd != -1) {
                std::map<int, Client>::iterator newOpIt = clientMap.find(newOpFd);
                if (newOpIt != clientMap.end()) {
                    std::string newOpNick = newOpIt->second.getNickname();
                    std::string modeMsg = ":" + SERVER_NAME + " MODE " + it->first + 
                                          " +o " + newOpNick + CLDR;
                    chan.broadcast(modeMsg);
                }
            }
        }
    }
    
    // Clean up client data
    clientMap.erase(client.fd);
    clientBuffer.erase(client.fd);
    
    // Close the socket
    close(client.fd);
    client.fd = -1;
    client.events = 0;
    client.revents = 0;
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
			
			std::string nick = clientObj.isNicknameSet() ? clientObj.getNickname() : "*";
			
			if (subCmd == "LS") {
				// Respond with supported capabilities (none for basic IRC)
				std::string response = ":" + SERVER_NAME + " CAP " + nick + " LS :" + CLDR;
				sendMessage(client, response);
			} else if (subCmd == "REQ") {
				// Acknowledge capability request (but we don't support any)
				std::string response = ":" + SERVER_NAME + " CAP " + nick + " ACK :" + CLDR;
				sendMessage(client, response);
			} else if (subCmd == "END") {
				// End capability negotiation
				// No response needed, client will proceed with registration
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
			cleanClient(client);
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
			std::string pongMsg = params.size() > 0 ? params[0] : SERVER_NAME;
			std::string response = ":" + SERVER_NAME + " PONG " + SERVER_NAME + " :" + pongMsg + CLDR;
			sendMessage(client, response);
			return ;
		}
		// Handle WHO command (WeeChat compatibility)
		if (cmd == "WHO") {
			if (params.size() < 1) {
				sendNumericReply(client, ERR_NEEDMOREPARAMS, "WHO :Not enough parameters");
				return;
			}
			std::string target = params[0];
			
			// If it's a channel
			if (target[0] == '#') {
				std::map<std::string, Channel>::iterator chanIt = channels.find(target);
				if (chanIt == channels.end()) {
					sendNumericReply(client, ERR_NOSUCHCHANNEL, target + " :No such channel");
					return;
				}
				
				Channel& chan = chanIt->second;
				const std::set<int>& members = chan.getMembers();
				
				// Send RPL_WHOREPLY (352) for each member
				for (std::set<int>::const_iterator it = members.begin(); it != members.end(); ++it) {
					std::map<int, Client>::iterator memIt = clientMap.find(*it);
					if (memIt != clientMap.end()) {
						Client& member = memIt->second;
						std::string flags = chan.isOperator(*it) ? "@" : "";
						std::string whoReply = ":" + SERVER_NAME + " 352 " + clientObj.getNickname() +
							" " + target + " " + member.getUsername() + " localhost " +
							SERVER_NAME + " " + member.getNickname() + " H" + flags +
							" :0 " + member.getRealname() + CLDR;
						sendMessage(client, whoReply);
					}
				}
			}
			
			// Send RPL_ENDOFWHO (315)
			std::string endWho = ":" + SERVER_NAME + " 315 " + clientObj.getNickname() +
				" " + target + " :End of /WHO list" + CLDR;
			sendMessage(client, endWho);
			return;
		}
		if (cmd == WEECHAT_QUIT){
			// Send QUIT message to all channels before cleaning
			if (this->clientMap.find(client.fd) != this->clientMap.end()) {
				Client& clientObj = this->clientMap[client.fd];
				std::string nickname = clientObj.getNickname();
				std::string reason = params.size() > 0 ? params[0] : "Client quit";
				
				// Broadcast to all channels the user is in
				for (std::map<std::string, Channel>::iterator it = channels.begin(); 
				     it != channels.end(); ++it) {
					Channel& chan = it->second;
					if (chan.hasMember(client.fd)) {
						std::string quitMsg = ":" + nickname + " QUIT :" + reason + CLDR;
						chan.broadcast(quitMsg);
					}
				}
			}
			cleanClient(client);
			return;
		}
		// Handle NAMES command
		if (cmd == "NAMES") {
			if (params.size() < 1) {
				// No parameter - could list all channels, but we'll just return end
				std::string endMsg = ":" + SERVER_NAME + " 366 " + 
					clientObj.getNickname() + " * :End of /NAMES list" + CLDR;
				sendMessage(client, endMsg);
				return;
			}
			
			std::string channelName = params[0];
			std::map<std::string, Channel>::iterator it = channels.find(channelName);
			
			if (it == channels.end()) {
				sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
				return;
			}
			
			Channel& chan = it->second;
			std::string namesReply = chan.getNamesReply(clientMap);
			sendMessage(client, namesReply);
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

		// If this is the first member, make them operator
		if (chan.getMemberCount() == 1) {
			chan.addOperator(client.fd);
			std::string opMsg = ":" + SERVER_NAME + " MODE " + channelName + " +o " + clientObj.getNickname() + CLDR;
    		sendMessage(client, opMsg);
		}

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
	// ============================================
    //  TOPIC COMMAND 
    // ============================================
	if (cmd == WEECHAT_TOPIC) {
        // Need at least channel name
        if (params.size() < 1) {
            sendNumericReply(client, ERR_NEEDMOREPARAMS, "TOPIC :Not enough parameters");
            return;
        }

        std::string channelName = params[0];
	        // Check if  exists
        std::map<std::string, Channel>::iterator it = channels.find(channelName);
        if (it == channels.end()) {
            sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
            return;
        }

        Channel& chan = it->second;

        // Client must be in thechannel channel
        if (!chan.hasMember(client.fd)) {
            sendNumericReply(client, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
            return;
        }

        // If only channel name provided: VIEW topic
        if (params.size() == 1) {
            std::string topic = chan.getTopic();
            
            if (topic.empty()) {
                // No topic set (331)
                sendNumericReply(client, RPL_NOTOPIC, channelName + " :No topic is set");
            } else {
                // Send topic (332)
                sendNumericReply(client, RPL_TOPIC, channelName + " :" + topic);
            }
            return;
        }

        // If topic provided: SET topic
        std::string newTopic = params[1];

        // Check if topic is restricted (+t mode)
        if (chan.isTopicRestricted() && !chan.isOperator(client.fd)) {
            sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
            return;
        }

        // Set the new topic
        chan.setTopic(newTopic);

        // Broadcast topic change to all channel members (including the setter)
        std::string topicMsg = ":" + clientObj.getNickname() + 
                               "!" + clientObj.getUsername() +
                               " TOPIC " + channelName + 
                               " :" + newTopic + CLDR;
        chan.broadcast(topicMsg);  // Broadcast to EVERYONE

        return;
    }
	// ============================================
	//  KICK COMMAND 
	// ============================================

	if (cmd == WEECHAT_KICK) {
    // KICK #channel nickname :reason
    // Need at least channel and nickname
    if (params.size() < 2) {
        sendNumericReply(client, ERR_NEEDMOREPARAMS, "KICK :Not enough parameters");
        return;
    }

    std::string channelName = params[0];
    std::string targetNick = params[1];
    std::string reason = "No reason given";
    
    if (params.size() >= 3) {
        reason = params[2];
    }

    // Check if channel exists
    std::map<std::string, Channel>::iterator it = channels.find(channelName);
    if (it == channels.end()) {
        sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
        return;
    }

    Channel& chan = it->second;

    // Kicker must be in the channel
    if (!chan.hasMember(client.fd)) {
        sendNumericReply(client, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
        return;
    }

    // Kicker must be a channel operator
    if (!chan.isOperator(client.fd)) {
        sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
        return;
    }

    // Find the target client by nickname
    int targetFd = -1;
    for (std::map<int, Client>::iterator clientIt = clientMap.begin(); 
         clientIt != clientMap.end(); ++clientIt) {
        if (clientIt->second.getNickname() == targetNick) {
            targetFd = clientIt->first;
            break;
        }
    }

    // Target must exist
    if (targetFd == -1) {
        sendNumericReply(client, ERR_NOSUCHNICK, targetNick + " :No such nick/channel");
        return;
    }

    // Target must be in the channel
    if (!chan.hasMember(targetFd)) {
        sendNumericReply(client, ERR_USERNOTINCHANNEL, targetNick + " " + channelName + " :They aren't on that channel");
        return;
    }

    // Build KICK message: :kicker!user KICK #channel target :reason
    std::string kickMsg = ":" + clientObj.getNickname() + 
                          "!" + clientObj.getUsername() + 
                          " KICK " + channelName + 
                          " " + targetNick + 
                          " :" + reason + CLDR;

    // Broadcast KICK to everyone in the channel (including the kicked user)
    chan.broadcast(kickMsg);

    // Remove the target from the channel and check for auto-promotion
    int newOpFd = chan.removeMember(targetFd);
    
    // If someone was auto-promoted, broadcast MODE +o
    if (newOpFd != -1) {
        std::map<int, Client>::iterator newOpIt = clientMap.find(newOpFd);
        if (newOpIt != clientMap.end()) {
            std::string newOpNick = newOpIt->second.getNickname();
            std::string modeMsg = ":" + SERVER_NAME + " MODE " + channelName + 
                                  " +o " + newOpNick + CLDR;
            chan.broadcast(modeMsg);
        }
    }

    return;
	}
	// ============================================
    //  PART COMMAND 
    // ============================================
    if (cmd == WEECHAT_PART) {
        // PART #channel :reason
        // Need at least channel name
        if (params.size() < 1) {
            sendNumericReply(client, ERR_NEEDMOREPARAMS, "PART :Not enough parameters");
            return;
        }

        std::string channelName = params[0];
        std::string reason = "Leaving";
        
        // Optional reason parameter
        if (params.size() >= 2) {
            reason = params[1];
        }

        // Check if channel exists
        std::map<std::string, Channel>::iterator it = channels.find(channelName);
        if (it == channels.end()) {
            sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
            return;
        }

        Channel& chan = it->second;

        // Must be in the channel to leave it
        if (!chan.hasMember(client.fd)) {
            sendNumericReply(client, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
            return;
        }

        // Build PART message: :nick!user PART #channel :reason
        std::string partMsg = ":" + clientObj.getNickname() + 
                              "!" + clientObj.getUsername() + 
                              " PART " + channelName + 
                              " :" + reason + CLDR;

        // Broadcast to everyone in channel (including the person leaving)
        chan.broadcast(partMsg);

        // Remove from channel and check for auto-promotion
        int newOpFd = chan.removeMember(client.fd);
        
        // If someone was auto-promoted, broadcast MODE +o
        if (newOpFd != -1) {
            std::map<int, Client>::iterator newOpIt = clientMap.find(newOpFd);
            if (newOpIt != clientMap.end()) {
                std::string newOpNick = newOpIt->second.getNickname();
                std::string modeMsg = ":" + SERVER_NAME + " MODE " + channelName + 
                                      " +o " + newOpNick + CLDR;
                chan.broadcast(modeMsg);
            }
        }

        return;
    }
// ============================================
    //  INVITE COMMAND 
    // ============================================
    if (cmd == WEECHAT_INVITE) {
        // INVITE <nickname> <channel>
        if (params.size() < 2) {
            sendNumericReply(client, ERR_NEEDMOREPARAMS, "INVITE :Not enough parameters");
            return;
        }

        std::string targetNick = params[0];
        std::string channelName = params[1];

        // Check if channel exists
        std::map<std::string, Channel>::iterator it = channels.find(channelName);
        if (it == channels.end()) {
            sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
            return;
        }

        Channel& chan = it->second;

        // Inviter must be in the channel
        if (!chan.hasMember(client.fd)) {
            sendNumericReply(client, ERR_NOTONCHANNEL, channelName + " :You're not on that channel");
            return;
        }

        // If channel is invite-only (+i), only operators can invite
        if (chan.isInviteOnly() && !chan.isOperator(client.fd)) {
            sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
            return;
        }

        // Find the target client by nickname
        int targetFd = -1;
        for (std::map<int, Client>::iterator clientIt = clientMap.begin(); 
             clientIt != clientMap.end(); ++clientIt) {
            if (clientIt->second.getNickname() == targetNick) {
                targetFd = clientIt->first;
                break;
            }
        }

        // Target must exist
        if (targetFd == -1) {
            sendNumericReply(client, ERR_NOSUCHNICK, targetNick + " :No such nick/channel");
            return;
        }

        // Target must NOT already be in the channel
        if (chan.hasMember(targetFd)) {
            sendNumericReply(client, ERR_USERONCHANNEL, targetNick + " " + channelName + " :is already on channel");
            return;
        }

        // Add target to the invited list
        chan.inviteUser(targetFd);

        // Send RPL_INVITING to the inviter (341)
        std::string invitingReply = ":" + SERVER_NAME + " 341 " + clientObj.getNickname() + 
                                    " " + targetNick + " " + channelName + CLDR;
        sendMessage(client, invitingReply);

        // Send INVITE message to the target user
        std::string inviteMsg = ":" + SERVER_NAME + " NOTICE " + targetNick + 
                                " :You have been invited to " + channelName + 
                                " by " + clientObj.getNickname() + CLDR;
        
        // Find the target's pollfd and send the message
        for (unsigned int i = 1; i < serverCapacity; i++) {
            if (clients[i].fd == targetFd) {
                sendMessage(clients[i], inviteMsg);
                break;
            }
        }

        return;
    }
	// ============================================
    //  MODE COMMAND 
    // ============================================
    if (cmd == WEECHAT_MODE) {
        // MODE #channel [+/-modes] [parameters]
        if (params.size() < 1) {
            sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
            return;
        }

        std::string target = params[0];

        // Check if it's a channel mode (starts with #)
        if (target[0] != '#') {
            // User mode - not implementing for this project
            return;
        }

        std::string channelName = target;

        // Check if channel exists
        std::map<std::string, Channel>::iterator it = channels.find(channelName);
        if (it == channels.end()) {
            sendNumericReply(client, ERR_NOSUCHCHANNEL, channelName + " :No such channel");
            return;
        }

        Channel& chan = it->second;

        // If no mode string provided, show current modes
        if (params.size() == 1) {
            std::string modes = "+";
            std::string modeParams = "";
            
            if (chan.isInviteOnly()) modes += "i";
            if (chan.isTopicRestricted()) modes += "t";
            if (!chan.getKey().empty()) {
                modes += "k";
                modeParams += " " + chan.getKey();
            }
			            if (chan.getUserLimit() > 0) {
                modes += "l";
                std::ostringstream oss;
                oss << chan.getUserLimit();
                modeParams += " " + oss.str();
            }
            
			// If no modes set, just show +
            if (modes == "+") {
                modes = "";
            }

            // RPL_CHANNELMODEIS (324)
            std::string reply = ":" + SERVER_NAME + " 324 " + clientObj.getNickname() + 
                               " " + channelName + " " + modes + modeParams + CLDR;
            sendMessage(client, reply);
            return;
        }

        // User must be operator to change modes
        if (!chan.isOperator(client.fd)) {
            sendNumericReply(client, ERR_CHANOPRIVSNEEDED, channelName + " :You're not channel operator");
            return;
        }

        std::string modeString = params[1];
        bool adding = true;  // + = adding mode, - = removing mode
        size_t paramIndex = 2;  // Index for mode parameters

        std::string addedModes = "";
        std::string removedModes = "";
        std::string addedParams = "";
        std::string removedParams = "";

        for (size_t i = 0; i < modeString.length(); i++) {
            char mode = modeString[i];

			if (mode == '+') {
                adding = true;
                continue;
            }
            if (mode == '-') {
                adding = false;
                continue;
            }

			switch (mode) {
                // +i: Invite-only channel
                case 'i': {
                    if (adding && !chan.isInviteOnly()) {
                        chan.setInviteOnly(true);
                        addedModes += 'i';
                    } else if (!adding && chan.isInviteOnly()) {
                        chan.setInviteOnly(false);
                        removedModes += 'i';
                    }
                    break;
                }

                // +t: Topic restricted to operators
                case 't': {
                    if (adding && !chan.isTopicRestricted()) {
                        chan.setTopicRestricted(true);
                        addedModes += 't';
                    } else if (!adding && chan.isTopicRestricted()) {
                        chan.setTopicRestricted(false);
                        removedModes += 't';
                    }
                    break;
                }

				
                // +k: Channel key (password)
                case 'k': {
                    if (adding) {
                        if (paramIndex >= params.size()) {
                            sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
                            return;
                        }
                        std::string key = params[paramIndex++];
                        chan.setKey(key);
                        addedModes += 'k';
                        addedParams += " " + key;
                    } else {
                        chan.setKey("");
                        removedModes += 'k';
                    }
                    break;
                }

                // +o: Give/take operator privilege
                case 'o': {
                    if (paramIndex >= params.size()) {
                        sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
                        return;
                    }
                    std::string targetNick = params[paramIndex++];
                    
                    // Find target user
                    int targetFd = -1;
                    for (std::map<int, Client>::iterator clientIt = clientMap.begin(); 
                         clientIt != clientMap.end(); ++clientIt) {
                        if (clientIt->second.getNickname() == targetNick) {
                            targetFd = clientIt->first;
                            break;
                        }
                    }
                    
                    if (targetFd == -1) {
                        sendNumericReply(client, ERR_NOSUCHNICK, targetNick + " :No such nick/channel");
                        continue;
                    }
                    
                    if (!chan.hasMember(targetFd)) {
                        sendNumericReply(client, ERR_USERNOTINCHANNEL, targetNick + " " + channelName + " :They aren't on that channel");
                        continue;
                    }
                    
                    if (adding && !chan.isOperator(targetFd)) {
                        chan.addOperator(targetFd);
                        addedModes += 'o';
                        addedParams += " " + targetNick;
                    } else if (!adding && chan.isOperator(targetFd)) {
                        chan.removeOperator(targetFd);
                        removedModes += 'o';
                        removedParams += " " + targetNick;
                    }
                    break;
                }

                // +l: User limit
                case 'l': {
                    if (adding) {
                        if (paramIndex >= params.size()) {
                            sendNumericReply(client, ERR_NEEDMOREPARAMS, "MODE :Not enough parameters");
                            return;
                        }
                        std::string limitStr = params[paramIndex++];
                        int limit = std::atoi(limitStr.c_str());
                        if (limit > 0) {
                            chan.setUserLimit(limit);
                            addedModes += 'l';
                            addedParams += " " + limitStr;
                        }
                    } else {
                        chan.setUserLimit(-1);
                        removedModes += 'l';
                    }
                    break;
                }

                default:
                    // Unknown mode, ignore
                    break;
            }
        }

        // Build the final mode string
        std::string appliedModes = "";
        std::string appliedParams = "";
        
        if (!addedModes.empty()) {
            appliedModes += "+" + addedModes;
            appliedParams += addedParams;
        }
        if (!removedModes.empty()) {
            appliedModes += "-" + removedModes;
            appliedParams += removedParams;
        }

        // Broadcast the mode change if any modes were applied
        if (!appliedModes.empty()) {
            std::string modeMsg = ":" + clientObj.getNickname() + 
                                  "!" + clientObj.getUsername() + 
                                  " MODE " + channelName + 
                                  " " + appliedModes + appliedParams + CLDR;
            chan.broadcast(modeMsg);
        }

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
	std::cout << rawMessage << std::endl;
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
 * @brief Gracefully shutdown the server
 * Called by signal handler to stop the event loop
 * 
 * @return void.
 * @author Hamad
 */
void	Server::shutdown(void){
	this->isRunning = false;
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
		
		// Handle poll errors (EINTR from signals is ok, continue)
		if (this->pollManager < 0){
			if (errno == EINTR)
				continue;  // Signal interrupted, check isRunning and continue
			break;  // Real error, exit loop
		}
		if (this->pollManager == 0){
			continue;  // Timeout, no events
		}
		if (this->clients[0].revents & POLLIN){
			int clientSocket = accept(this->clients[0].fd, NULL, NULL);
			if (clientSocket < 0)
				continue ;
			
			// Set the new client socket to non-blocking
			if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) < 0) {
				close(clientSocket);
				continue;
			}
			
			// Check if server is full (serverCapacity includes server socket at index 0)
			if (this->clientMap.size() >= serverCapacity - 1){
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
				
				// Empty buffer means client disconnected or error
				if (buffer.empty()){
					cleanClient(client);
					continue;
				}
				
				// Check if client still exists (might have been removed)
				if (this->clientBuffer.find(client.fd) == this->clientBuffer.end())
					continue;
				
				std::string& clientBuffer = this->clientBuffer[client.fd];
				clientBuffer += buffer;
				size_t endPosition = clientBuffer.find(CLDR);
				while (endPosition != std::string::npos){
					std::string message = clientBuffer.substr(0, endPosition);
					// Use RFC-compliant message handler
					handleMessage(client, message);
					
					// Check if client still valid after handling message
					if (client.fd < 0)
						break;
					
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

