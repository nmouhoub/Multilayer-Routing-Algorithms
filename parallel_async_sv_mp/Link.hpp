/*
 *   Copyright (c) 2023 Noureddine Mouhoub

 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.

 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.

 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef LINK_HPP
#define LINK_HPP


#include <utility> 
#include <unordered_map>

#include "ConcurrentQueue.hpp"
#include "AdaptationFunction.hpp"
#include "Message.hpp"

using namespace std;

typedef unordered_map<AdaptationFunction*, int> MapCost;


class Link
{
	private : 
		int src; 
		int dest; 
		MapCost map_cost;
		ConcurrentQueue<Message*> messages_queue; 
	public :
		Link(int _src, int _dest);
		int get_src();
		int get_dest();	
		MapCost get_map_cost();
		void set_map_cost(MapCost _map_cost);
		int get_cost(AdaptationFunction *adapt_func);
		bool is_empty();
		Message* get_message();
		void set_message(Message *msg);
};


#endif