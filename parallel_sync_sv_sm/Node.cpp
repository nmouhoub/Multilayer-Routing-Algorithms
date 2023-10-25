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
	nb_opers = 0;
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

void Node::init_routing_table()
{
	for(auto f : adapt_functions)
	{
		routing_table->add_route(id, string(1,f->get_from()), 0, id, f, 0, 0, 0);
	}
}

void Node::increment_nb_opers()
{
	nb_opers += 1;
}

long long int Node::get_nb_opers()
{
	return nb_opers;
}

Node::~Node()
{
	delete routing_table;
}