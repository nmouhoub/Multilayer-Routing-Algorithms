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

#include "Node.hpp"

Node::Node(int _id)
{
	id = _id;
	routing_table = new RoutingTable();
}

void Node::add_neighbor(int _neighbor)
{
	neighbors.insert(_neighbor);
}

void Node::add_neighbor_pred(int _neighbor)
{
	neighbors_pred.insert(_neighbor);
}

void Node::add_neighbor_succ(int _neighbor)
{
	neighbors_succ.insert(_neighbor);
}

void Node::set_neighbors_id(unordered_set<int> _neighbors)
{
	neighbors = _neighbors;
}

void Node::add_adapt_func(AdaptationFunction *adapt_func)
{
	adapt_functions.insert(adapt_func);
	protocols_in.insert(adapt_func->get_from());
	protocols_out.insert(adapt_func->get_to());
}

void Node::set_adapt_functions(unordered_set<AdaptationFunction*> _adapt_functions)
{
	adapt_functions = _adapt_functions;
	for(auto f : adapt_functions)
	{
		if(f->get_type() == DC)
		{
			protocols_in.insert(f->get_to());
			protocols_out.insert(f->get_from());
		}
		protocols_in.insert(f->get_from());
		protocols_out.insert(f->get_to());	
	}
}

int Node::get_id()
{
	return id;
}

unordered_set<int> Node::get_neighbors_id()
{
	return neighbors;
}

unordered_set<int> Node::get_neighbors_pred_id()
{
	return neighbors_pred;
}

unordered_set<int> Node::get_neighbors_succ_id()
{
	return neighbors_succ;
}

unordered_set<char> Node::get_protocols_in()
{
	return protocols_in;
}

unordered_set<char> Node::get_protocols_out()
{
	return protocols_out;
}

RoutingTable* Node::get_routing_table()
{
	return routing_table;
}

unordered_set<AdaptationFunction *> Node::get_adapt_functions()
{
	return adapt_functions;
}


bool Node::if_protocol_in(char x) 
{
	unordered_set<char>::iterator it = protocols_in.find(x);
	return ( it != protocols_in.end() ) ;
}


void Node::init_routing_table()
{
	for(auto f : adapt_functions)
	{
		int h_max;
		string stack_out, stack_in; 
		if (f->get_type() == 0)
		{
	    	h_max = 1;
	    	stack_out = string(1,f->get_from());
	    	stack_in = string(1,f->get_to());
		}
		else
		{
			if (f->get_type() == 1)
			{
	    		h_max = 2;
		    	stack_out = string(1,f->get_from());
		    	stack_in = string(1,f->get_from());
		    	stack_in.push_back(f->get_to());
			}
			else
			{
				h_max = 2;
	    		stack_out = string(1,f->get_to());
	    		stack_out.push_back(f->get_from());
	    		stack_in = string(1,f->get_to());
			}
		}
		routing_table->add_route(id, stack_out, stack_in, 0, id, f, h_max, 0);
	}
}



Node::~Node()
{
	delete routing_table;
}