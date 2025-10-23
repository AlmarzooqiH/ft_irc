/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hamalmar <hamalmar@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/23 09:46:19 by hamalmar          #+#    #+#             */
/*   Updated: 2025/10/23 14:08:34 by hamalmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include "UtilityHeaders.hpp"
# include "SocketHeaders.hpp"


/**
 * @note Will extract the proper data feilds for the class from the irc  protocol
 * manual later on.
 * 
 * @author Hamad
*/

class Channel{
    private:
        std::string channelName;
        std::string topic;
        std::map<int, pollfd> channelMembers;

    public:
        Channel();
        ~Channel();
        Channel(const Channel& right);
        Channel& operator=(const Channel& right);

        std::string getChannelName(void) const;
        std::string getTopic(void) const;

        void    setChannelName(const std::string& nName);
        void    setTopic(const std::string& nTopic);
};

#endif