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


Network::Network(int _nb_nodes, int _nb_links, unordered_set<char> _protocols, int _max_stack)
{
	nb_nodes = _nb_nodes;
	nb_links = _nb_links;

	old_buff = new vector<array<list<Tuple>,2>>(_nb_nodes);
	new_buff = new vector<array<list<Tuple>,2>>(_nb_nodes);

	tab_threads = new vector<thread>(_nb_nodes);
	tab_changed = new vector<bool>(_nb_nodes);


    m_count = _nb_nodes; 
    m_generation =0;
    m_count_reset_value = _nb_nodes;

	protocols = _protocols;
	max_stack = _max_stack;
	finished = false;
	nb_iters = 0;

	nb_hops = new float[max_stack]();
	nb_costs = new float[max_stack]();

	net_diameter = new int[max_stack]();

	number_feasible_routes = new long long int[max_stack]();
	number_unfeasible_routes = new long long int[max_stack]();

	total_nb_found_routes = 0;
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

void Network::init_step(Node *n)
{
	for(auto func : n->get_adapt_functions())
	{
		if (func->get_type() != 1)
		{
			for(auto v_id : n->get_neighbors_succ_id()) 
			{
				for(auto x : get_node(v_id)->get_protocols_in())
				{
					n->increment_nb_opers();
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
								if (h_max <= max_stack)
								{
									bool changed = n->get_routing_table()->add_route(v_id, protocol_stack, cost, v_id, func, h_max, 1, 0);
									if (changed == true)
									{
										if (protocol_stack.back() == 'a')
										{
											new_buff->at(n->get_id())[0].push_front(make_tuple(v_id, protocol_stack, cost, h_max, 1));	
										}else
										{
											new_buff->at(n->get_id())[1].push_front(make_tuple(v_id, protocol_stack, cost, h_max, 1));
										}
										tab_changed->at(n->get_id()) = true;
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


void Network::sync_step()
{
	unique_lock<mutex> lock(mtx);
    unsigned int gen = m_generation;

    if (--m_count == 0)
    {
    	nb_iters++;
    	update_finished();
        update_buffers();
      	m_generation++;
      	m_count = m_count_reset_value;
      	cv.notify_all();
      	return;
    }

    while (gen == m_generation)
      	cv.wait(lock);
}


void Network::main_step(Node *n)
{
	while (!finished)
	{
		tab_changed->at(n->get_id()) = false;

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
					
					n->increment_nb_opers();

					if(rev_func->apply_to_stack(new_stack))
					{
						if( (!new_stack.empty()) && ((int) new_stack.size() <= max_stack) )
						{
							Link *link = get_link(n->get_id(), v_id);
				    		if (link != nullptr)
				    		{
								int new_cost = get<2>(*it) + link->get_cost(func);
								int h_max = get<3>(*it);
								//int hops = get<4>(*it) +1;
								int hops = get<4>(*it);
								if (new_stack.size() == 1)
								{
									hops = hops + 1;
								}
								if ((int) new_stack.size() > h_max) 
								{
										h_max = (int) new_stack.size();
								}

								bool changed = n->get_routing_table()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, h_max, hops, nb_iters);
								if (changed == true)
								{
									if (new_stack.back() == 'a')
									{
										new_buff->at(n->get_id())[0].push_front(make_tuple(get<0>(*it), new_stack, new_cost, h_max, hops));
									}else
									{
										new_buff->at(n->get_id())[1].push_front(make_tuple(get<0>(*it), new_stack, new_cost, h_max, hops));
									}

									if (tab_changed->at(n->get_id()) == false) 
									{
										tab_changed->at(n->get_id()) = true;
									}
								}
							}
						}
					}
					delete rev_func;
				}
			}
		}

		//synch and wait other threads
		sync_step();
	}
}



void Network::start_node(Node *n)
{
	// initialization step
	init_step(n);
	
	// synchronization step
	sync_step();
	
	// main step 
	main_step(n);
	
	//add self loops at the end 
	//n->init_routing_table();
}

void Network::start()
{
	// create and start all threads
	for (auto n: nodes)
	{
		tab_threads->at(n->get_id()) = thread(&Network::start_node,this,n);
	}

	// wait all node threads 
	for (vector<thread>::iterator v_iter = tab_threads->begin(); v_iter != tab_threads->end(); v_iter++)
	{
		if ( (*v_iter).joinable() )  
		{
			(*v_iter).join();	
		}
	}
}


void Network::update_finished()
{
    finished = all_of(tab_changed->begin(), tab_changed->end(), [](bool elt){return (elt==false);});
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


float Network::get_number_paths()
{
	float nb_paths = 0.0;
	set<int> all_paths_node;
	float n = (float) get_nodes().size();

	for(auto n : nodes)
	{	
		unordered_map<Index,Row> table = n->get_routing_table()->get_table();
		for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); it++)
  		{
  			int dest = it->first.first;
  			string protocol_stack = it->first.second; 
  			if ((protocol_stack.size() == 1) && (dest != n->get_id()))
  			{
				all_paths_node.insert(dest);
			}
		}
		nb_paths += (float) all_paths_node.size();
		all_paths_node.clear();
	}
	
	float result = nb_paths / ((n*(n-1))) * 100;
	return result;
}


float * Network::get_number_hops()
{
	int *hops = new int[max_stack]();
	int *total_nb_paths = new int[max_stack](); 

	for(auto n : nodes)
	{	
		unordered_map<Index,Row> table = n->get_routing_table()->get_table();
		for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); it++)
  		{
  			int dest = it->first.first;
  			string protocol_stack = it->first.second; 
  			if ((protocol_stack.size() == 1) && (dest != n->get_id()))
  			{  
				int h_max = it->second.first.first.second;
  				int hops_path = it->second.first.second;
				
				total_nb_paths[h_max-1] += 1;
				hops[h_max-1] += hops_path;
			}
		}
	}
	for (int i = 0; i < max_stack; i++)
	{
		nb_hops[i] = (float) hops[i] / (float) total_nb_paths[i];
	}
	return nb_hops;	
}

/*float Network::get_number_hops()
{
	int hops = 0;
	int total_nb_paths = 0; 

	for(auto n : nodes)
	{	
		unordered_map<Index,Row> table = n->get_routing_table()->get_table();
		for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); it++)
  		{
  			int dest = it->first.first;
  			string protocol_stack = it->first.second; 
  			if ((protocol_stack.size() == 1) && (dest != n->get_id()))
  			{  
  				int hops_path = it->second.first.second;
				total_nb_paths += 1;
				hops += hops_path;
			}
		}
	}
	float result = (float) hops / (float) total_nb_paths;
	if (isnan(result)) { result = 0.0; }
	return result;

}*/


int * Network::get_net_diameter()
{
	//number_hops();
	return net_diameter;
}


float * Network::get_nb_costs()
{
	return nb_costs;
}




int Network::get_nb_nodes()
{
	return nb_nodes;
}

int Network::get_nb_links()
{
	return nb_links;
}


int Network::get_number_iters()
{
	return nb_iters;
}

long long int Network::get_number_opers()
{
	long long int nb_opers = 0; 
	for(auto n : nodes)
	{
		nb_opers += n->get_nb_opers();
	}
	return nb_opers;
}

long long int * Network::get_number_feasible_routes()
{
	for(auto n : nodes)
	{	
		unordered_map<Index,Row> table = n->get_routing_table()->get_table();
		for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); it++)
  		{
  			string protocol_stack = it->first.second; 
			int h_max = it->second.first.first.second; 
  			if ((protocol_stack.size() == 1))
  			{  
				number_feasible_routes[h_max-1] += 1;
			}
			else
			{
				number_unfeasible_routes[h_max-1] += 1;
			}
			total_nb_found_routes +=1;
		}
	}
	return number_feasible_routes;	
}

long long int * Network::get_number_unfeasible_routes()
{
	return number_unfeasible_routes;
}


long long int Network::get_number_found_routes()
{
	return total_nb_found_routes;
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

	delete tab_threads; 
	delete tab_changed;
}