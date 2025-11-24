#ifndef UTILITIYFUNCTIONS_HPP
# define UTILITIYFUNCTIONS_HPP
# include "UtilityHeaders.hpp"
# include "SocketHeaders.hpp"
# include "Constants.hpp"

std::string	recieveData(pollfd& client);
void        sendMessage(pollfd& client, const std::string& message);
void        channelSendMessage(int clientFd, const std::string& message);
#endif