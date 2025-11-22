/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ialashqa <ialashqa@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 15:02:39 by ialashqa          #+#    #+#             */
/*   Updated: 2025/11/01 15:02:39 by ialashqa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# include "UtilityHeaders.hpp"


class Channel {
    private:
        std::string channelName;
        std::string topic;
        std::string createdAt;
        std::set<int> channelMembers;  // Client FDs (only store FDs to avoid nickname issues)
        std::set<int> operators;       // Operator FDs (only store FDs)
        std::string key;               // Channel password (+k mode)
        int userLimit;                 // -1 = no limit (+l mode)
        bool inviteOnly;               // +i mode
        bool topicRestricted;          // +t mode (only ops can change topic)

    public:
        // Constructors and destructor
        Channel();
        Channel(const std::string& name);
        Channel(const Channel& copy); // private
        Channel& operator=(const Channel& right); // private
        ~Channel();

        // Member management
        void addMember(int clientFd);
        void removeMember(int clientFd);
        bool hasMember(int clientFd) const;
        const std::set<int>& getMembers() const;

        // Operator management
        void addOperator(int clientFd);
        void removeOperator(int clientFd);
        bool isOperator(int clientFd) const;

        // Getters
        std::string getName() const;
        std::string getTopic() const;
        bool isInviteOnly() const;
        bool isTopicRestricted() const;
        std::string getKey() const;
        int getUserLimit() const;
        
        // Setters
        void setTopic(const std::string& newTopic);
        void setInviteOnly(bool value);
        void setTopicRestricted(bool value);
        void setKey(const std::string& password);
        void setUserLimit(int limit);

};

# endif