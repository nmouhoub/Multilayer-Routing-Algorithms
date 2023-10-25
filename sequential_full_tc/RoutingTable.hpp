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

#include <unordered_map>
#include "AdaptationFunction.hpp"

typedef pair<int,pair<string,string>> Index;
typedef pair<pair<pair<pair<int,int>,AdaptationFunction*>,int>,int> Row;


namespace std {
	template <> struct hash<std::pair<int, std::pair<string,string>>> {
	    inline size_t operator()(const std::pair<int, std::pair<string,string>> &v) const {
	        std::hash<int> int_hasher;
	        std::hash<string> string_hasher;
	        return (int_hasher(v.first) ^ string_hasher(v.second.first) ^ string_hasher(v.second.second));
	    }
	};
}


class RoutingTable
{
	private: 
		unordered_map<Index,Row> table; 

	public:
		RoutingTable();
		void display();
		unordered_map<Index,Row> get_table();
		int get_cost(int dest, string stack_out, string stack_in);
		int get_next_hop(int dest, string stack_out, string stack_in);
		AdaptationFunction *get_adapt_func(int dest, string stack_out, string stack_in);
		bool add_route(int dest, string stack_out, string stack_in, int cost, int next_hop, AdaptationFunction *adapt_func, int h_max, int i);
};

#endif