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

void Network::start()
{
	init();
	apply_neighborhood();
	apply_transitive_closure();
	add_self_loops();
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
									bool changed = n->get_routing_table()->add_route(v_id, protocol_stack, cost, v_id, func, x, 0);
									if (changed == true)
									{
										if (protocol_stack.back() == 'a')
										{
											old_buff->at(n->get_id())[0].push_front(make_tuple(v_id, protocol_stack, cost, x));
											if (protocol_stack.size() == 1)
											{
												n->get_routing_table_a()->add_route(v_id, protocol_stack, cost, v_id, func, x, 0);
											}	
										}else
										{
											old_buff->at(n->get_id())[1].push_front(make_tuple(v_id, protocol_stack, cost, x));
											if (protocol_stack.size() == 1)
											{
												n->get_routing_table_b()->add_route(v_id, protocol_stack, cost, v_id, func, x, 0);
											}
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



void Network::apply_neighborhood()
{
	int i = 1; 
	while (!finished)
	{
		finished = true;
		for(auto n : nodes) 
		{
			for(auto func : n->get_adapt_functions())
			{
				for(auto v_id : n->get_neighbors_succ_id()) 
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
						string new_stack = get<1>(*it);
						if (new_stack.size() != 1)
						{
							AdaptationFunction *rev_func = func->reverse();	
							if(rev_func->apply_to_stack(new_stack))
							{
								if( (!new_stack.empty()) && ((int) new_stack.size() <= max_stack) )
								{
									Link *link = get_link(n->get_id(), v_id);
						    		if (link != nullptr)
						    		{
										int new_cost = get<2>(*it) + link->get_cost(func);
										bool changed = n->get_routing_table()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, get<3>(*it), i);
										if (changed == true)
										{
											if (new_stack.back() == 'a')
											{
												new_buff->at(n->get_id())[0].push_front(make_tuple(get<0>(*it), new_stack, new_cost, get<3>(*it)));
												if (new_stack.size() == 1)
												{
													n->get_routing_table_a()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, get<3>(*it), 0);
												}
											}else
											{
												new_buff->at(n->get_id())[1].push_front(make_tuple(get<0>(*it), new_stack, new_cost, get<3>(*it)));
												if (new_stack.size() == 1)
												{
													n->get_routing_table_b()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, get<3>(*it), 0);
												}
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
		}

		//update buffers 
		update_buffers();
		
		i++;
	}
	nb_iters = i;
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



void Network::apply_transitive_closure_node(RoutingTable *routing_table, int i)
{
	unordered_map<Index,Row> table_1, table_2;
	
	table_1 = routing_table->get_table();
	for (unordered_map<Index,Row>::iterator n_it=table_1.begin(); n_it!=table_1.end(); n_it++)
	{
		int d = n_it->first.first;
		char x_in = n_it->second.first.second;
		
		if (x_in == 'a') {
			table_2 = get_node(d)->get_routing_table_a()->get_table();
		}else
		{
			table_2 = get_node(d)->get_routing_table_b()->get_table();
		}

		for (unordered_map<Index,Row>::iterator d_it=table_2.begin(); d_it!=table_2.end(); d_it++)
		{
			int iter_1 = n_it->second.second;
			int iter_2 = d_it->second.second;

			if ( (iter_1 == i-1) || (iter_2 == i-1) )
			{
				int new_dest = d_it->first.first;
				string new_stack = n_it->first.second;
				int new_cost = n_it->second.first.first.first.first + d_it->second.first.first.first.first;
				int next_hop = n_it->second.first.first.first.second; 
				AdaptationFunction *adapt_func = n_it->second.first.first.second;
				char new_x_in = d_it->second.first.second;
				bool changed = routing_table->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, i);
				if((changed == true) && (finished == true))
				{
					finished = false;
				}
			}
		}
	}
}

void Network::apply_transitive_closure()
{			
	int i = 1;
	finished = false;
	
	while (!finished)
	{
		finished = true;

		for(auto n : nodes)
		{
			// for routes with H=a
			apply_transitive_closure_node(n->get_routing_table_a(), i);

  			// for routes with H=b
  			apply_transitive_closure_node(n->get_routing_table_b(), i);
		}
		i++;
	}
	nb_iters = nb_iters + i;
}


void Network::add_self_loops()
{
	for(auto n : nodes) 
	{
		n->init_routing_table();
	}
}

void Network::display_all_routing_tables()
{
	for(auto n : nodes)
	{
		cout << "routing table of node: " << n->get_id() << endl;
		cout << "=========================" << endl;
		n->get_routing_table_a()->display();
		n->get_routing_table_b()->display();
		cout << endl;
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