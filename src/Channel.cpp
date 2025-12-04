/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ialashqa <ialashqa@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 14:07:54 by ialashqa          #+#    #+#             */
/*   Updated: 2025/11/26 16:31:39 by ialashqa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel() : 
    channelName(""),
    topic(""),
    createdAt(""),
    invitedUsers(),
    channelMembers(),
    inviteOnly(false),
    topicRestricted(true),
    moderated(false),
    key(""),
    userLimit(-1),
    operators(),
    voicedUsers(),
    banMasks(),
    exceptionMasks()
{}

Channel::Channel(const std::string& name) : 
    channelName(name),
    topic(""),
    createdAt(""),
    invitedUsers(),
    channelMembers(),
    inviteOnly(false),
    topicRestricted(true),
    moderated(false),
    key(""),
    userLimit(-1),
    operators(),
    voicedUsers(),
    banMasks(),
    exceptionMasks()
{
    std::time_t currentTime = std::time(NULL); 
    std::ostringstream oss;
    oss << currentTime;
    createdAt = oss.str();
}

Channel::Channel(const Channel& copy) {
    *this = copy;
}

Channel& Channel::operator=(const Channel& right){
    if (this != &right){
        this->channelName = right.channelName;
        this->topic = right.topic;
        this->createdAt = right.createdAt;
        this->invitedUsers = right.invitedUsers;
        this->channelMembers = right.channelMembers;
        this->inviteOnly = right.inviteOnly;           
        this->topicRestricted = right.topicRestricted;
        this->moderated = right.moderated;
        this->key = right.key;
        this->userLimit = right.userLimit;
        this->operators = right.operators;
        this->voicedUsers = right.voicedUsers;
        this->banMasks = right.banMasks;
        this->exceptionMasks = right.exceptionMasks;
	}
	return (*this);
}

Channel::~Channel(){}

/* ---------------------------------------------- */
/*            Member Management                   */
/* ---------------------------------------------- */

/**
 * @brief Add a new member to the channel.
 * @param clientFd The file descriptor of the client to add.
 * @note add client fd to channelMembers set (list).
 */
void Channel::addMember(int clientFd) {
    channelMembers.insert(clientFd);
}

/**
 * @brief Remove a member from the channel + Auto-promote operator.
 * @param clientFd The file descriptor of the client to remove.
 * @note If no operators remain in channel, the first member is promoted to operator.
 */
int Channel::removeMember(int clientFd) {
    channelMembers.erase(clientFd);
    operators.erase(clientFd);
    invitedUsers.erase(clientFd);
    
    // Auto-promote if no operators left
    if (operators.empty() && !channelMembers.empty()) {
        int newOp = *channelMembers.begin();
        addOperator(newOp);
        return newOp;  // Return the new operator's fd
    }
    return -1;  // No promotion happened
}

/**
 * @brief Check if a specific client (fd) is in the channel.
 * @param clientFd The file descriptor of the client to check.
 * @return True if in the channel, false otherwise.
 */
bool Channel::hasMember(int clientFd) const {
    std::set<int>::const_iterator it = channelMembers.find(clientFd);
    bool found = (it != channelMembers.end());
    return found;
}

//Getter for private variable channelMembers.
const std::set<int>& Channel::getMembers() const {
    return channelMembers;
}

/**
 * @brief Get the count of members in the channel.
 * @return Number of members.
 */
size_t Channel::getMemberCount() const {
    return channelMembers.size();
}

/* ---------------------------------------------- */
/*         Invitation Management                  */
/* ---------------------------------------------- */

/**
 * @brief Invite a user to the channel.
 * @param clientFd The file descriptor of the client to invite.
 * @note Adds client fd to invitedUsers set (list).
 */
void Channel::inviteUser(int clientFd) {
    invitedUsers.insert(clientFd);
}

/**
 * @brief Check if a user is invited to the channel.
 * @param clientFd The file descriptor of the client to check.
 * @return True if invited, false otherwise.
 */
bool Channel::isInvited(int clientFd) const {
    return invitedUsers.find(clientFd) != invitedUsers.end();
}

/**
 * @brief Remove an invitation for a user.
 * @param clientFd The file descriptor of the client to remove invitation for.
 * @note Removes client fd from invitedUsers set (list).
 */
void Channel::removeInvite(int clientFd) {
    invitedUsers.erase(clientFd);
}

/* ---------------------------------------------- */
/*          Operator Management                   */
/* ---------------------------------------------- */

void Channel::addOperator(int clientFd) {
    if (hasMember(clientFd)) {
        operators.insert(clientFd);
    }
}

void Channel::removeOperator(int clientFd) {
    operators.erase(clientFd);
}

bool Channel::isOperator(int clientFd) const {
    return operators.find(clientFd) != operators.end();
}

const std::set<int>& Channel::getOperators() const {
    return operators;
}

size_t Channel::getOperatorCount() const {
    return operators.size();
}

/* ---------------------------------------------- */
/*            Broadcasting Messages               */
/* ---------------------------------------------- */

/**
 * @brief Sends a message to every member in the channel.
 * @param message The message to broadcast.
 */
void Channel::broadcast(const std::string& message) {
    std::set<int>::iterator it = channelMembers.begin();
    std::set<int>::iterator end = channelMembers.end();

    while (it != end) {
        int clientfd = *it;
        const char *msg = message.c_str();
        channelSendMessage(clientfd, msg);
        ++it;
    }
}


/**
 * @brief Sends a message to every member in the channel except one (the sender obv)
 * @param message The message to broadcast.
 * @param excludeFd The file descriptor of the client to exclude.
 */
void Channel::broadcast(const std::string& message, int excludeFd) {
    std::set<int>::iterator it = channelMembers.begin();
    std::set<int>::iterator end = channelMembers.end();

    while (it != end) {
        int clientfd = *it;

        if (clientfd != excludeFd) {
            const char *msg = message.c_str();
            channelSendMessage(clientfd, msg);
        }
        ++it;
    }
}

/* ---------------------------------------------- */
/*         Channel Info & Replies                 */
/* ---------------------------------------------- */

/**
 * @brief Generate the list of usernames with operators marked with @ prefix.
 * @param clientMap A map of client FDs with their Client objects with nicknames.
 * @return The formatted NAMES reply string.
 * @note Operators are prefixed with '@' in the reply e.g. @bob.
 */
std::string Channel::getNamesReply(const std::map<int, Client>& clientMap) const {
    std::string names;
    
    // Loop through all operators first
    for (std::set<int>::const_iterator it = operators.begin(); it != operators.end(); ++it) {
        std::map<int, Client>::const_iterator clientIt = clientMap.find(*it);
        if (clientIt != clientMap.end()) {
            if (!names.empty()) names += " ";
            names += "@" + clientIt->second.getNickname(); // give nickname with @ prefix for ops
        }
    }
    
    // Regular members (exclude operators)
    for (std::set<int>::const_iterator it = channelMembers.begin(); it != channelMembers.end(); ++it) {
        if (operators.find(*it) != operators.end())
            continue;  // Skip operators, already added
        
        std::map<int, Client>::const_iterator clientIt = clientMap.find(*it);
        if (clientIt != clientMap.end()) {
            if (!names.empty()) names += " ";
            names += clientIt->second.getNickname(); // give nickname without @ prefix for reg
        }
    }
    
    // Build the full NAMES reply with proper format
    std::string reply;
    
    // RPL_NAMREPLY (353): :server 353 nick = #channel :names
    reply += ":" + SERVER_NAME + " 353 = " + channelName + " :" + names + CLDR;
    
    // RPL_ENDOFNAMES (366): :server 366 nick #channel :End of /NAMES list
    reply += ":" + SERVER_NAME + " 366 " + channelName + " :End of /NAMES list" + CLDR;
    
    return reply;
}

std::string Channel::getName() const {
    return channelName;
}

std::string Channel::getTopic() const {
    return topic;
}

std::string Channel::getCreationTime() const {
    return createdAt;
}

/* ---------------------------------------------- */
/*              Mode Getters                      */
/* ---------------------------------------------- */

bool Channel::isInviteOnly() const {
    return inviteOnly;
}

bool Channel::isTopicRestricted() const {
    return topicRestricted;
}

bool Channel::isModerated() const {
    return moderated;
}

std::string Channel::getKey() const {
    return key;
}

int Channel::getUserLimit() const {
    return userLimit;
}

/* ---------------------------------------------- */
/*              Mode Setters                      */
/* ---------------------------------------------- */

void Channel::setInviteOnly(bool value) {
    inviteOnly = value;
}

void Channel::setTopicRestricted(bool value) {
    topicRestricted = value;
}

void Channel::setModerated(bool value) {
    moderated = value;
}

void Channel::setKey(const std::string& password) {
    key = password;
}

void Channel::setUserLimit(int limit) {
    userLimit = limit;
}

void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

/* ---------------------------------------------- */
/*              Voice Management (+v)             */
/* ---------------------------------------------- */

void Channel::addVoice(int clientFd) {
    voicedUsers.insert(clientFd);
}

void Channel::removeVoice(int clientFd) {
    voicedUsers.erase(clientFd);
}

bool Channel::hasVoice(int clientFd) const {
    return voicedUsers.find(clientFd) != voicedUsers.end();
}

/* ---------------------------------------------- */
/*              Ban Management (+b)               */
/* ---------------------------------------------- */

void Channel::addBan(const std::string& mask) {
    banMasks.insert(mask);
}

void Channel::removeBan(const std::string& mask) {
    banMasks.erase(mask);
}

// Check if userhost matches any ban mask
bool Channel::isBanned(const std::string& userhost) const {
    for (std::set<std::string>::const_iterator it = banMasks.begin(); 
         it != banMasks.end(); ++it) {
        if (matchWildcard(*it, userhost)) {
            return true;
        }
    }
    return false;
}

const std::set<std::string>& Channel::getBanList() const {
    return banMasks;
}

/* ---------------------------------------------- */
/*           Exception Management (+e)            */
/* ---------------------------------------------- */

void Channel::addException(const std::string& mask) {
    exceptionMasks.insert(mask);
}

void Channel::removeException(const std::string& mask) {
    exceptionMasks.erase(mask);
}

// Check if userhost matches any exception mask
bool Channel::matchesException(const std::string& userhost) const {
    for (std::set<std::string>::const_iterator it = exceptionMasks.begin(); 
         it != exceptionMasks.end(); ++it) {
        if (matchWildcard(*it, userhost)) {
            return true;
        }
    }
    return false;
}

const std::set<std::string>& Channel::getExceptionList() const {
    return exceptionMasks;
}