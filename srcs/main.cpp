/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ahamalai <ahamalai@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/03 14:22:40 by ahamalai          #+#    #+#             */
/*   Updated: 2024/09/03 14:58:54 by ahamalai         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ServerInfo.hpp"
#include <iostream>

unsigned int convertip(std::string ip);

int	main(void)
{
	std::cout << convertip("192.168.0.1") << std::endl;
}
