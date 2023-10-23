#include "RoutingTable.hpp"

RoutingTable::RoutingTable()
{
	table = unordered_map<Index,Row>();
}

void RoutingTable::display()
{
	for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); ++it)
  	{
  		int dest = it->first.first;
  		string protocol_stack = it->first.second; 
  		int cost = it->second.first.first.first.first;
  		int next_hop = it->second.first.first.first.second;
  		AdaptationFunction *adapt_func = it->second.first.first.second;
  		cout << "dest: " << dest << " ";
  		cout << "stack: " << protocol_stack << " ";
  		cout << "cost: " << cost << " ";
  		cout << "next_hop: " << next_hop << " ";
  		cout << "function: " ;
  		adapt_func->display();
  		cout << "===============================================================" << endl;
  	}
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
		return it->second.first.first.first.first;
	return -1;
}

int RoutingTable::get_next_hop(int dest, string protocol_stack)
{
	Index id = make_pair(dest,protocol_stack); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it != table.end())
		return it->second.first.first.first.second;
	return -1;
}

char RoutingTable::get_protocol_in(int dest, string protocol_stack)
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
		return it->second.first.first.second;
	return nullptr;
}

bool RoutingTable::add_route(int dest, string protocol_stack, int cost, int next_hop, AdaptationFunction *adapt_func, char protocol_in, int i)
{
	Index id = make_pair(dest,protocol_stack); 
	unordered_map<Index,Row>::iterator it = table.find(id); 
	if (it == table.end())
	{
		Row row = make_pair(make_pair(make_pair(make_pair(cost, next_hop), adapt_func),protocol_in),i);
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
			it->second.first.second = protocol_in;
			it->second.second = i;
			return true;
		}
	}
	return false;
}