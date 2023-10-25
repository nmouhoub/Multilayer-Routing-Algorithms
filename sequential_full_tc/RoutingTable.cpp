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

#include "RoutingTable.hpp"

RoutingTable::RoutingTable()
{
	table = unordered_map<Index,Row>();
}

void RoutingTable::display()
{
	for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); ++it)
  	{
  		// Index
  		int dest = it->first.first;
  		string stack_out = it->first.second.first;
  		string stack_in = it->first.second.second;

  		// Row
  		int cost = it->second.first.first.first.first;
  		int next_hop = it->second.first.first.first.second;
  		int h_max = it->second.first.second;
  		AdaptationFunction *adapt_func = it->second.first.first.second;
  		cout << "dest: " << dest << " ";
  		cout << "stack_out: " << stack_out << " ";
  		cout << "stack_in: " << stack_in << " ";
  		cout << "cost: " << cost << " ";
  		cout << "next_hop: " << next_hop << " ";
  		cout << "h_max: " << h_max << " ";
  		cout << "function: " ;
  		adapt_func->display();
  		cout << "===============================================================" << endl;
  	}
}


unordered_map<Index,Row> RoutingTable::get_table() 
{
	return table;
}

int RoutingTable::get_cost(int dest, string stack_out, string stack_in)
{
	Index id = make_pair(dest,make_pair(stack_out,stack_in)); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it != table.end())
		return it->second.first.first.first.first;
	return -1;
}

int RoutingTable::get_next_hop(int dest, string stack_out, string stack_in)
{
	Index id = make_pair(dest,make_pair(stack_out,stack_in)); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it != table.end())
		return it->second.first.first.first.second;
	return -1;
}


AdaptationFunction *RoutingTable::get_adapt_func(int dest, string stack_out, string stack_in)
{
	Index id = make_pair(dest,make_pair(stack_out,stack_in)); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it != table.end())
		return it->second.first.first.second;
	return nullptr;
}

bool RoutingTable::add_route(int dest, string stack_out, string stack_in, int cost, int next_hop, AdaptationFunction *adapt_func, int h_max, int i)
{
	Index id = make_pair(dest,make_pair(stack_out,stack_in));  
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it == table.end())
	{
		Row row = make_pair(make_pair(make_pair(make_pair(cost, next_hop), adapt_func),h_max),i);
		table.insert(make_pair(id,row));
		return true;
	}
	else 
	{	
		if ((it->second.first.first.first.first) > cost) 
		{
			it->second.first.first.first.first = cost;
			it->second.first.first.first.second = next_hop;
			it->second.first.first.second = adapt_func;
			it->second.first.second = h_max;
			it->second.second = i;
			return true;
		}
	}
	return false;
}