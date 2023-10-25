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
	finished = false;
	nb_iters = 0;
	nb_operations = 0;
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
	apply_transitive_closure();
	//add_self_loops();
}

void Network::init()
{
	int cost, h_max;
	string stack_out, stack_in;

	for(auto n : nodes) 
	{
		for(auto v_id : n->get_neighbors_succ_id()) 
		{
			Link *link = get_link(n->get_id(),v_id);
			if (link != nullptr)
			{
				for(auto func : n->get_adapt_functions())
				{
					if ( ( func->get_type() == 0 ) && ( get_node(v_id)->if_protocol_in(func->get_to()) ) )  
				    {
				    	h_max = 1;
				    	cost = link->get_cost(func);
				    	stack_out = string(1,func->get_from());
				    	stack_in = string(1,func->get_to());
				    	n->get_routing_table()->add_route(v_id, stack_out, stack_in, cost, v_id, func, h_max, 0);
				    }
				    else
				    {
				    	if ( ( func->get_type() == 1 ) && ( get_node(v_id)->if_protocol_in(func->get_to()) ) )  
				    	{
				    		h_max = 2;
				    		cost = link->get_cost(func);
					    	stack_out = string(1,func->get_from());
					    	stack_in = string(1,func->get_from());
					    	stack_in.push_back(func->get_to());
					    	n->get_routing_table()->add_route(v_id, stack_out, stack_in, cost, v_id, func, h_max, 0);
				    	}
				    	else
				    	{
				    		h_max = 2;
				    		cost = link->get_cost(func);
				    		stack_out = string(1,func->get_to());
				    		stack_out.push_back(func->get_from());
				    		stack_in = string(1,func->get_to());
				    		n->get_routing_table()->add_route(v_id, stack_out, stack_in, cost, v_id, func, h_max, 0);
				    	}
				    }
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
				unordered_map<Index,Row> table_1, table_2;
		        int iter_1, iter_2;		

				string stack_out_1,stack_in_1, stack_out_2, stack_in_2; 
				string new_stack_out, new_stack_in,tmp_stack;

				int d, cost_1,cost_2,new_cost; 
				int h_max_1, h_max_2,new_h_max;

	            AdaptationFunction *adapt_func;
	            int new_dest, next_hop;
				
				table_1 = n->get_routing_table()->get_table();
				for (unordered_map<Index,Row>::iterator n_it=table_1.begin(); n_it!=table_1.end(); n_it++)
				{
					d = n_it->first.first;
					table_2 = get_node(d)->get_routing_table()->get_table();

					for (unordered_map<Index,Row>::iterator d_it=table_2.begin(); d_it!=table_2.end(); d_it++)
					{
						iter_1 = n_it->second.second;
						iter_2 = d_it->second.second;

						if ( (iter_1 == i-1) || (iter_2 == i-1) )
						{
							stack_out_1 = n_it->first.second.first; 
							stack_in_1   = n_it->first.second.second; 
							stack_out_2  = d_it->first.second.first;
							stack_in_2 = d_it->first.second.second; 

							cost_1 = n_it->second.first.first.first.first;;
							cost_2 = d_it->second.first.first.first.first;;

							h_max_1 = n_it->second.first.second;
							h_max_2 = d_it->second.first.second;  

							if (stack_in_1.compare(stack_out_2) == 0)  // case 1 
							{
								new_dest = d_it->first.first; 
								new_stack_out = stack_out_1;
								new_stack_in = stack_in_2;

								new_h_max = max(h_max_1,h_max_2);
								new_cost = cost_1 + cost_2;
								next_hop = n_it->second.first.first.first.second;
								adapt_func = n_it->second.first.first.second;
								if (new_h_max <= max_stack)
								{
									bool changed = n->get_routing_table()->add_route(new_dest, new_stack_out, new_stack_in, new_cost, next_hop, adapt_func, new_h_max, i);
									if((changed == true) && (finished == true))
									{
										finished = false;
									}
								}
								nb_operations++;
							}
							else
							{
								if ((int) (stack_in_1.size()-stack_out_2.size()) > 0)
								{
									if (stack_in_1.compare(stack_in_1.size()-stack_out_2.size(),stack_out_2.size(),stack_out_2) == 0)                // case 2
									{
										new_dest = d_it->first.first; 
										new_stack_out = stack_out_1;
										tmp_stack = stack_in_1.substr(0,stack_in_1.size()-stack_out_2.size());
										new_stack_in = tmp_stack.append(stack_in_2);
										new_h_max = max(h_max_1,h_max_2+(int)tmp_stack.size());
										new_cost = cost_1 + cost_2;
										next_hop = n_it->second.first.first.first.second;
										adapt_func = n_it->second.first.first.second;
										
										if (new_h_max <= max_stack)
										{
											bool changed = n->get_routing_table()->add_route(new_dest, new_stack_out, new_stack_in, new_cost, next_hop, adapt_func, new_h_max, i);
											if((changed == true) && (finished == true))
											{
												finished = false;
											}
										}
										nb_operations++;
									}
								}
								else
								{ 
									if (stack_out_2.compare(stack_out_2.size()-stack_in_1.size(),stack_in_1.size(),stack_in_1) == 0)            // case 3
									{
										new_dest = d_it->first.first; 
										tmp_stack = stack_out_2.substr(0,stack_out_2.size()-stack_in_1.size());
										new_stack_out = tmp_stack.append(stack_out_1);
										new_stack_in = stack_in_2;
										new_h_max = max(h_max_1+(int)tmp_stack.size(),h_max_2);
										new_cost = cost_1 + cost_2;
										next_hop = n_it->second.first.first.first.second;
										adapt_func = n_it->second.first.first.second;
										
										if (new_h_max <= max_stack)
										{
											bool changed = n->get_routing_table()->add_route(new_dest, new_stack_out, new_stack_in, new_cost, next_hop, adapt_func, new_h_max, i);
											if((changed == true) && (finished == true))
											{
												finished = false;
											}
										}
										nb_operations++;
									}
								}
							}
						}
					}
				}
		}
		i++;
	}
	nb_iters = i;
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
		n->get_routing_table()->display();
		cout << endl;
	}
}

int Network::get_number_iters()
{
	return nb_iters;
}

int Network::get_number_opers()
{
	return nb_operations;
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