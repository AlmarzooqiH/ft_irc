/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 14:07:54 by ialashqa          #+#    #+#             */
/*   Updated: 2025/11/24 21:47:56 by hamalmar         ###   ########.fr       */
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
    key(""),
    userLimit(-1),
    operators()
{}

Channel::Channel(const std::string& name) : 
    channelName(name),
    topic(""),
    createdAt(""),
    invitedUsers(),
    channelMembers(),
    inviteOnly(false),
    topicRestricted(true),
    key(""),
    userLimit(-1),
    operators()
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
        this->inviteOnly = right.inviteOnly;           // FIXED: Order now matches header
        this->topicRestricted = right.topicRestricted; // FIXED: Order now matches header
        this->key = right.key;
        this->userLimit = right.userLimit;
        this->operators = right.operators;
	}
	return (*this);
}

Channel::~Channel(){}

/* ---------------------------------------------- */
/*            Member Management                   */
/* ---------------------------------------------- */

void Channel::addMember(int clientFd) {
    channelMembers.insert(clientFd);
}

void Channel::removeMember(int clientFd) {
    channelMembers.erase(clientFd);
    operators.erase(clientFd);
    invitedUsers.erase(clientFd);
    
    // Auto-promote if no operators left
    if (operators.empty() && !channelMembers.empty()) {
        int newOp = *channelMembers.begin();
        addOperator(newOp);
    }
}

bool Channel::hasMember(int clientFd) const {
    return channelMembers.find(clientFd) != channelMembers.end();
}

const std::set<int>& Channel::getMembers() const {
    return channelMembers;
}

size_t Channel::getMemberCount() const {
    return channelMembers.size();
}

/* ---------------------------------------------- */
/*         Invitation Management                  */
/* ---------------------------------------------- */

void Channel::inviteUser(int clientFd) {
    invitedUsers.insert(clientFd);
}

bool Channel::isInvited(int clientFd) const {
    return invitedUsers.find(clientFd) != invitedUsers.end();
}

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

void Channel::broadcast(const std::string& message) {
    for (std::set<int>::iterator it = channelMembers.begin(); it != channelMembers.end(); ++it) {
        channelSendMessage(*it, message.c_str());
    }
}

void Channel::broadcast(const std::string& message, int excludeFd) {
    for (std::set<int>::iterator it = channelMembers.begin(); it != channelMembers.end(); ++it) {
        if (*it != excludeFd)
        channelSendMessage(*it, message.c_str());
    }
}

/* ---------------------------------------------- */
/*         Channel Info & Replies                 */
/* ---------------------------------------------- */

std::string Channel::getNamesReply(const std::map<int, Client>& clientMap) const {
    std::string names;
    
    // Operators first with @
    for (std::set<int>::const_iterator it = operators.begin(); it != operators.end(); ++it) {
        std::map<int, Client>::const_iterator clientIt = clientMap.find(*it);
        if (clientIt != clientMap.end()) {
            if (!names.empty()) names += " ";
            names += "@" + clientIt->second.getNickname();
        }
    }
    
    // Regular members (exclude operators)
    for (std::set<int>::const_iterator it = channelMembers.begin(); it != channelMembers.end(); ++it) {
        if (operators.find(*it) != operators.end())
            continue;  // Skip operators, already added
        
        std::map<int, Client>::const_iterator clientIt = clientMap.find(*it);
        if (clientIt != clientMap.end()) {
            if (!names.empty()) names += " ";
            names += clientIt->second.getNickname();
        }
    }
    
    return names;
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

void Channel::setKey(const std::string& password) {
    key = password;
}

void Channel::setUserLimit(int limit) {
    userLimit = limit;
}

void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}