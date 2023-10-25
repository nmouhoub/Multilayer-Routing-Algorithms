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

#ifndef ROUTING_TABLE_HPP
#define ROUTING_TABLE_HPP

#include <mutex>
#include <unordered_map>
#include "AdaptationFunction.hpp"

typedef pair<int,string> Index;
typedef pair<pair<pair<pair<pair<int,int>,AdaptationFunction*>,char>,int>,int> Row;


namespace std {
	template <> struct hash<std::pair<int, string>> {
	    inline size_t operator()(const std::pair<int, string> &v) const {
	        std::hash<int> int_hasher;
	        std::hash<string> string_hasher;
	        return int_hasher(v.first) ^ string_hasher(v.second);
	    }
	};
}


class RoutingTable
{
	private: 
		unordered_map<Index,Row> table; 
	public:
		RoutingTable();
		unordered_map<Index,Row> get_table();
		int get_cost(int dest, string protocol_stack);
		int get_next_hop(int dest, string protocol_stack);
		char get_protocol_in(int dest, string protocol_stack);
		AdaptationFunction *get_adapt_func(int dest, string protocol_stack);
		bool add_route(int dest, string protocol_stack, int cost, int next_hop, AdaptationFunction *adapt_func, char protocol_in, int h_max, int i);
};

#endif
