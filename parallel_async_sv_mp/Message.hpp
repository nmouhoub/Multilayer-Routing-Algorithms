#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <stack>

using namespace std;

class Message 
{
	private : 
	    int dest;
	    int cost;
	    string protocol_stack; 
	public : 
		Message(int _dest, string _protocol_stack, int _cost);
		int get_dest();
		int get_cost();
		string get_stack();
		void display();
};


#endif