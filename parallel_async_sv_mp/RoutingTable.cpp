#include "RoutingTable.hpp"

RoutingTable::RoutingTable()
{
	table = unordered_map<Index,Row>();
}


unordered_map<Index,Row> RoutingTable::get_table() 
{
	return table;
}

int RoutingTable::get_cost(int dest, string protocol_stack)
{
	Index id = make_pair(dest,protocol_stack); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it != table.end())
		return it->second.first.first;
	return -1;
}

int RoutingTable::get_next_hop(int dest, string protocol_stack)
{
	Index id = make_pair(dest,protocol_stack); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it != table.end())
		return it->second.first.second;
	return -1;
}

AdaptationFunction *RoutingTable::get_adapt_func(int dest, string protocol_stack)
{
	Index id = make_pair(dest,protocol_stack); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it != table.end())
		return it->second.second;
	return nullptr;
}

bool RoutingTable::add_route(int dest, string protocol_stack, int cost, int next_hop, AdaptationFunction *adapt_func)
{
	Index id = make_pair(dest,protocol_stack); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it == table.end())
	{
		Row row = make_pair(make_pair(cost, next_hop), adapt_func);
		table.insert(make_pair(id,row));
		return true;
	}
	else 
	{	
		if ((it->second.first.first) > cost) 
		{
			it->second.first.first = cost;
			it->second.first.second = next_hop;
			it->second.second = adapt_func;
			return true;
		}
	}
	return false;
}