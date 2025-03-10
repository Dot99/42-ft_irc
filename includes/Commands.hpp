/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gude-jes <gude-jes@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/10 12:18:45 by gude-jes          #+#    #+#             */
/*   Updated: 2025/03/10 12:57:43 by gude-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Client.hpp"

class Commands
{
	private:
		/* data */
	public:
		Commands();
		~Commands();
		void parseCommand(std::string command, std::string param, int client_fd, Client *client);
};