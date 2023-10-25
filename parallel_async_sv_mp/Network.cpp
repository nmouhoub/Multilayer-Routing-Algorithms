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

#include "Network.hpp"


Network::Network(unordered_set<char> _protocols, int _max_stack)
{
	protocols = _protocols;
	max_stack = _max_stack;
	total_nb_message = 0;
}

int Network::get_max_stack()
{
	return max_stack;
}

long long int Network::get_nb_message()
{
	return total_nb_message;
}

unordered_set<Node*> Network::get_nodes()
{
	return nodes;
}

unordered_set<Link*> Network::get_links()
{
	return links;
}

Node* Network::get_node(int _id)
{
	for(auto n : nodes)
	{
		if (n->get_id() == _id)
		{
			return n;
		}
	}	
	return nullptr;
}

unordered_set<char> Network::get_protocols()
{
	return protocols;
}

unordered_set<AdaptationFunction*> Network::get_adapt_functions()
{
	return adapt_functions;
}

void Network::set_nodes(unordered_set<Node*> _nodes)
{
	nodes = _nodes;
}

void Network::set_links(unordered_set<Link*> _links)
{
	links = _links;
}

void Network::set_adapt_functions(unordered_set<AdaptationFunction*> _adapt_functions)
{
	adapt_functions = _adapt_functions;
}

void Network::start()
{
	for(auto n : nodes)
	{
		n->start();
	}
}

void Network::wait()
{
	// Give time for messages processing on worker threads
	this_thread::sleep_for(chrono::milliseconds(1));
	std::vector<bool> are_empty(nodes.size());
	do 
	{	int i = 0;
		for (auto n : nodes) {
			are_empty[i] = n->get_queue()->empty();
			i++;
		}
	}
	while(!all_of(are_empty.begin(), are_empty.end(), [](bool elt){return (elt==true);}) );
}

void Network::stop()
{
	for(auto n : nodes)
	{
		n->stop();
		total_nb_message += n->get_nb_message();
	}
}

Network::~Network() 
{	
	for(auto n : nodes) 
		delete n;
	for(auto l : links)
		delete l;
	for(auto f : adapt_functions) 
		delete f;
}