#include "Message.hpp"


Message::Message(int _dest, string _protocol_stack, int _cost)
{
	dest = _dest; 
	cost = _cost;
	protocol_stack = _protocol_stack;
}

string Message::get_stack()
{
	return protocol_stack;
}

int Message::get_dest()
{
	return dest;
}

int Message::get_cost()
{
	return cost;
}

void Message::display()
{
	cout << "===============" << endl;
	cout << "dest: " <<  dest << endl; 
	cout << "cost: " << cost << endl; 
	cout << "stack: " << protocol_stack << endl; 
	cout << "===============" << endl;
}