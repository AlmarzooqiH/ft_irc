/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ialashqa <ialashqa@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 14:07:54 by ialashqa          #+#    #+#             */
/*   Updated: 2025/11/01 14:07:54 by ialashqa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel() : 
    channelName(""),
    topic(""),
    key(""),
    userLimit(-1),
    inviteOnly(false),
    topicRestricted(true)
{}

Channel::Channel(const std::string& name) : 
    channelName(name),
    topic(""),
    key(""),
    userLimit(-1),
    inviteOnly(false),
    topicRestricted(true)
{
    std::time_t currentTime = std::time(NULL); 
    std::ostringstream oss;
    oss << currentTime;
    // oss << std::put_time(std::localtime(&currentTime), "%Y-%m-%d %H:%M:%S");
    createdAt = oss.str();
}

Channel::Channel(const Channel& copy) {
    *this = copy;
}

Channel& Channel::operator=(const Channel& right){
    if (this != &right){
        this->channelName = right.channelName;
        this->topic = right.topic;
        this->channelMembers = right.channelMembers;
        this->operators = right.operators;
        this->key = right.key;
        this->userLimit = right.userLimit;
        this->inviteOnly = right.inviteOnly;
        this->topicRestricted = right.topicRestricted;
	}
	return (*this);
}

Channel::~Channel(){}

void Channel::addMember(int clientFd) {
    channelMembers.insert(clientFd);
}

void Channel::removeMember(int clientFd) {
    channelMembers.erase(clientFd);
    operators.erase(clientFd);
}

bool Channel::hasMember(int clientFd) const {
    return channelMembers.find(clientFd) != channelMembers.end();
}

const std::set<int>& Channel::getMembers() const {
    return channelMembers;
}

// Operator management
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

// Getters
std::string Channel::getName() const {
    return channelName;
}

std::string Channel::getTopic() const {
    return topic;
}

std::string Channel::getCreationTime() const {
    return createdAt;
}

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

// Setters
void Channel::setTopic(const std::string& newTopic) {
    topic = newTopic;
}

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

