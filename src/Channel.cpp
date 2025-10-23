/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 11:17:31 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/23 11:33:05 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"

Channel::Channel():
channelName(""),
topic("")
{}
Channel::~Channel(){}

Channel::Channel(const Channel& right):
channelName(right.channelName),
topic(right.topic),
channelMembers(right.channelMembers)
{}

Channel& Channel::operator=(const Channel& right){
	if (this != &right){
		this->channelName = right.channelName;
		this->topic = right.topic;
		this->channelMembers = right.channelMembers;
	}
	return (*this);
}
