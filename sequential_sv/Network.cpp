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


Network::Network(int nb_nodes, unordered_set<char> _protocols, int _max_stack)
{
	old_buff = new vector<array<list<Tuple>,2>>(nb_nodes);
	new_buff = new vector<array<list<Tuple>,2>>(nb_nodes);

	protocols = _protocols;
	max_stack = _max_stack;
	finished = false;
	nb_iters = 0;
}

int Network::get_max_stack()
{
	return max_stack;
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

Link *Network::get_link(int src, int dest)
{
	for(auto l : links)
	{
		if ((l->get_src() == src) && (l->get_dest() == dest))
		{
			return l;
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

void Network::init()
{
	for(auto n : nodes) 
	{
		for(auto func : n->get_adapt_functions())
		{
			if (func->get_type() != 1)
			{
				for(auto v_id : n->get_neighbors_succ_id()) 
				{
					for(auto x : get_node(v_id)->get_protocols_in())
					{

						AdaptationFunction *rev_func = func->reverse();
						string protocol_stack = string(1,x);
						if(rev_func->apply_to_stack(protocol_stack))
						{
							if( !protocol_stack.empty() )
							{
								Link *link = get_link(n->get_id(),v_id);
							    if (link != nullptr)
							    {
									int cost = link->get_cost(func);
									int h_max = int (protocol_stack.size());
									bool changed = n->get_routing_table()->add_route(v_id, protocol_stack, cost, v_id, func, 1, h_max, 0);
									if (changed == true)
									{
										if (protocol_stack.back() == 'a')
										{
											old_buff->at(n->get_id())[0].push_front(make_tuple(v_id, protocol_stack, cost, h_max, 1));	
										}else
										{
											old_buff->at(n->get_id())[1].push_front(make_tuple(v_id, protocol_stack, cost, h_max, 1));
										}
									}							
								}
							}
						}
						delete rev_func;
					}
				}
			}
		}
	}
} 

void Network::start()
{
	init();
	int i = 1; 
	while (!finished)
	{
		finished = true;
		for(auto n : nodes) 
		{
			for(auto v_id : n->get_neighbors_succ_id())
			{
				for(auto func : n->get_adapt_functions()) 
				{
					list<Tuple> old_tmp;
					if (func->get_to() == 'a')
					{
						old_tmp = old_buff->at(v_id)[0];
					}else
					{
						old_tmp = old_buff->at(v_id)[1];
					}
					

					for(list<Tuple>::iterator it=old_tmp.begin(); it!=old_tmp.end(); it++)
					{
						AdaptationFunction *rev_func = func->reverse();
						string new_stack = get<1>(*it);
						if(rev_func->apply_to_stack(new_stack))
						{
							if( (!new_stack.empty()) && ((int) new_stack.size() <= max_stack) )
							{
								Link *link = get_link(n->get_id(), v_id);
					    		if (link != nullptr)
					    		{
									int new_cost = get<2>(*it) + link->get_cost(func);
									int new_hops = get<3>(*it) + 1;
									int h_max = get<4>(*it);
									
									if ((int) new_stack.size() > h_max) 
									{
										h_max = (int) new_stack.size();
									}

									bool changed = n->get_routing_table()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, new_hops, h_max, i);
									if (changed == true)
									{
										if (new_stack.back() == 'a')
										{
											new_buff->at(n->get_id())[0].push_front(make_tuple(get<0>(*it), new_stack, new_cost, new_hops, h_max));
										}else
										{
											new_buff->at(n->get_id())[1].push_front(make_tuple(get<0>(*it), new_stack, new_cost, new_hops, h_max));
										}

										if (finished == true) 
										{
											finished = false;
										}
									}
								}
							}
						}
						delete rev_func;
					}
				}
			}
		}

		//update buffers 
		update_buffers();
		
		i++;
	}
	
	nb_iters =i; 

	for(auto n : nodes) 
	{
		n->init_routing_table();
	}
}



void Network::update_buffers()
{
	tmp_buff = old_buff;
	old_buff = new_buff;
	new_buff = tmp_buff;

	for (vector<array<list<Tuple>,2>>::iterator v_iter = new_buff->begin(); v_iter != new_buff->end(); v_iter++)
	{
		(*v_iter)[0].clear();
		(*v_iter)[1].clear();	
	}
}

int Network::get_number_iters()
{
	return nb_iters;
}

Network::~Network() 
{	
	for(auto n : nodes) 
		delete n;
	for(auto l : links)
		delete l;
	for(auto f : adapt_functions) 
		delete f;
	
	delete old_buff;
	delete new_buff; 
}