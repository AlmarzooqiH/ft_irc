/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ialashqa <ialashqa@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/01 15:02:39 by ialashqa          #+#    #+#             */
/*   Updated: 2025/11/30 12:05:57 by ialashqa         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <set>
# include "UtilityHeaders.hpp"
# include "Client.hpp"
# include "UtilitiyFunctions.hpp"

class Client; 

class Channel {
    private:
        std::string channelName;
        std::string topic;
        std::string createdAt;
        std::set<int> invitedUsers; 
        std::set<int> channelMembers;  // Client FDs (only store FDs to avoid nickname issues)
        bool inviteOnly;               // +i mode
        bool topicRestricted;          // +t mode (only ops can change topic)
        bool moderated;                // +m mode (only ops and voiced can speak)
        std::string key;               // Channel password (+k mode)
        int userLimit;                 // -1 = no limit (+l mode)
        std::set<int> operators;       // Operator FDs (+o mode)
        std::set<int> voicedUsers;     // Voiced users (+v mode)
        std::set<std::string> banMasks;      // Ban list (+b mode)
        std::set<std::string> exceptionMasks; // Exception list (+e mode)
        
    public:
        // Constructors and destructor
        Channel();
        Channel(const std::string& name);
        Channel(const Channel& copy); // private
        Channel& operator=(const Channel& right); // private
        ~Channel();
        
        // Member management
        void addMember(int clientFd);
        int removeMember(int clientFd);
        bool hasMember(int clientFd) const;
        const std::set<int>& getMembers() const;
        size_t getMemberCount() const;
        
        // Invitation management
        void inviteUser(int clientFd);
        bool isInvited(int clientFd) const;
        void removeInvite(int clientFd);
        
        // Operator management
        void addOperator(int clientFd);
        void removeOperator(int clientFd);
        bool isOperator(int clientFd) const;
        const std::set<int>& getOperators() const;
        size_t getOperatorCount() const;
        
        // Voice management (+v mode)
        void addVoice(int clientFd);
        void removeVoice(int clientFd);
        bool hasVoice(int clientFd) const;
        
        // Ban management (+b mode)
        void addBan(const std::string& mask);
        void removeBan(const std::string& mask);
        bool isBanned(const std::string& userhost) const;
        const std::set<std::string>& getBanList() const;
        
        // Exception management (+e mode)
        void addException(const std::string& mask);
        void removeException(const std::string& mask);
        bool matchesException(const std::string& userhost) const;
        const std::set<std::string>& getExceptionList() const;
        
        // Broadcasting messages
        void broadcast(const std::string& message);
        void broadcast(const std::string& message, int excludeFd);
        
        // Channel info and replies
        std::string getNamesReply(const std::map<int, Client>& clientMap) const;
        std::string getName() const;
        std::string getTopic() const;
        std::string getCreationTime() const;
        

        // Mode Getters
        bool isInviteOnly() const;          // +i mode
        bool isTopicRestricted() const;     // +t mode
        bool isModerated() const;           // +m mode
        std::string getKey() const;         // +k mode
        int getUserLimit() const;           // +l mode
            
        // Mode Setters
        void setInviteOnly(bool value);                  // +i mode
        void setTopicRestricted(bool value);             // +t mode
        void setModerated(bool value);                   // +m mode
        void setKey(const std::string& password);        // +k mode
        void setUserLimit(int limit);                    // +l mode
        void setTopic(const std::string& newTopic);      // Topic content 
};

# endif