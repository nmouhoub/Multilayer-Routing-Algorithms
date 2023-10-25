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

	old_buff_tc = new vector<array<list<Tuple>,4>>(nb_nodes);
	new_buff_tc = new vector<array<list<Tuple>,4>>(nb_nodes);
	
	tab_threads = new vector<thread>(nb_nodes);
	tab_changed = new vector<bool>(nb_nodes);

    m_count = nb_nodes; 
    m_generation =0;
    m_count_reset_value = nb_nodes;

	protocols = _protocols;
	max_stack = _max_stack;
	
	finished_nei = false;
	finished_tc = false;
	
	nb_iters_nei = 0;
	nb_iters_tc = 1;
	nb_iters = 0;

	total_nb_opers_nei = 0;
	total_nb_opers_tc = 0;

	number_unfeasible_routes = new long long int[max_stack](); 
	number_feasible_routes = new long long int[max_stack](); 

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
					AdaptationFunction *rev_func = func->reverse();
					string protocol_stack = string(1,x);
					if(rev_func->apply_to_stack(protocol_stack))
					{
						if((!protocol_stack.empty()) && ((int) protocol_stack.size() <= max_stack)) 
						{
							Link *link = get_link(n->get_id(),v_id);
						    if (link != nullptr)
						    {
								n->increment_nb_opers_nei();

								int cost = link->get_cost(func);
								int h_max = protocol_stack.size();
								bool changed = n->get_routing_table()->add_route(v_id, protocol_stack, cost, v_id, func, x, h_max, 0);
								if (changed == true)
								{
									if (protocol_stack.back() == 'a')
									{
										new_buff->at(n->get_id())[0].push_front(make_tuple(v_id, protocol_stack, cost, x, h_max, v_id, func));
										if (protocol_stack.size() == 1)
										{
											if (x == 'a') 
											{
												n->get_routing_table_a_a()->add_route(v_id, protocol_stack, cost, v_id, func, x, h_max, 0);
												old_buff_tc->at(n->get_id())[0].push_front(make_tuple(v_id, protocol_stack, cost, x, h_max, v_id, func));
											}
											else
											{
												n->get_routing_table_a_b()->add_route(v_id, protocol_stack, cost, v_id, func, x, h_max, 0);
												old_buff_tc->at(n->get_id())[1].push_front(make_tuple(v_id, protocol_stack, cost, x, h_max, v_id, func));
											}
										}	
									}else
									{
										new_buff->at(n->get_id())[1].push_front(make_tuple(v_id, protocol_stack, cost, x, h_max, v_id, func));
										if (protocol_stack.size() == 1)
										{
											if (x == 'a')
											{
												n->get_routing_table_b_a()->add_route(v_id, protocol_stack, cost, v_id, func, x, h_max, 0);
												old_buff_tc->at(n->get_id())[3].push_front(make_tuple(v_id, protocol_stack, cost, x, h_max, v_id, func));
											}
											else
											{
												n->get_routing_table_b_b()->add_route(v_id, protocol_stack, cost, v_id, func, x, h_max, 0);
												old_buff_tc->at(n->get_id())[2].push_front(make_tuple(v_id, protocol_stack, cost, x, h_max, v_id, func));
											}
											
										}
									}
									tab_changed->at(n->get_id()) = true;
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

void Network::sync_step_nei()
{
    
	/*
	unsigned int gen = m_generation.load();
	
    if (--m_count == 0)
    {
      if (m_generation.compare_exchange_weak(gen, gen + 1))
      {
      	nb_iters++;
        update_finished();
        update_buffers();
        m_count = m_count_reset_value;
      }
      return;
    }

    while ((gen == m_generation) && (m_count != 0))
    	this_thread::yield();
    */

	
	unique_lock<mutex> lock(mtx);
    unsigned int gen = m_generation;

    if (--m_count == 0)
    {
    	nb_iters_nei++;
    	update_finished_nei();
        update_buffers();
      	m_generation++;
      	m_count = m_count_reset_value;
      	cv.notify_all();
      	return;
    }

    while (gen == m_generation)
      cv.wait(lock);	
}


void Network::sync_step_tc()
{
    
	/*
	unsigned int gen = m_generation.load();
	
    if (--m_count == 0)
    {
      if (m_generation.compare_exchange_weak(gen, gen + 1))
      {
      	nb_iters++;
        update_finished();
        update_buffers();
        m_count = m_count_reset_value;
      }
      return;
    }

    while ((gen == m_generation) && (m_count != 0))
    	this_thread::yield();
    */

	
	unique_lock<mutex> lock(mtx);
    unsigned int gen = m_generation;

    if (--m_count == 0)
    {
    	nb_iters_tc++;
    	update_finished_tc();
		update_buffers_tc();
    	update_routing_tables();
      	m_generation++;
      	m_count = m_count_reset_value;
      	cv.notify_all();
      	return;
    }

    while (gen == m_generation)
      cv.wait(lock);	
}



void Network::neighborhood_step(Node *n)
{
	while (!finished_nei)
	{
		tab_changed->at(n->get_id()) = false;

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
									n->increment_nb_opers_nei(); 
									int new_cost = get<2>(*it) + link->get_cost(func);
									char x = get<3>(*it);
									int h_max = get<4>(*it);
									if((int) new_stack.size() > h_max)
									{
										h_max = new_stack.size();
									}
									bool changed = n->get_routing_table()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, x, h_max, nb_iters);
									if (changed == true)
									{
										if (new_stack.back() == 'a')
										{
											new_buff->at(n->get_id())[0].push_front(make_tuple(get<0>(*it), new_stack, new_cost, x, h_max, v_id, func));
											if (new_stack.size() == 1)
											{
												if (x == 'a')
												{
													n->get_routing_table_a_a()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, x, h_max, 0);
													old_buff_tc->at(n->get_id())[0].push_front(make_tuple(get<0>(*it), new_stack, new_cost, x, h_max, v_id, func));
												}
												else
												{
													n->get_routing_table_a_b()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, x, h_max, 0);
													old_buff_tc->at(n->get_id())[1].push_front(make_tuple(get<0>(*it), new_stack, new_cost, x, h_max, v_id, func));
												}
											}
										}else
										{
											new_buff->at(n->get_id())[1].push_front(make_tuple(get<0>(*it), new_stack, new_cost, x, h_max, v_id, func));
											if (new_stack.size() == 1)
											{
												if ( x == 'a')
												{
													n->get_routing_table_b_a()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, x, h_max, 0);
													old_buff_tc->at(n->get_id())[3].push_front(make_tuple(get<0>(*it), new_stack, new_cost, x, h_max, v_id, func));
												}
												else
												{
													n->get_routing_table_b_b()->add_route(get<0>(*it), new_stack, new_cost, v_id, func, x, h_max, 0);
													old_buff_tc->at(n->get_id())[2].push_front(make_tuple(get<0>(*it), new_stack, new_cost, x, h_max, v_id, func));
												}
											}
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
		}

		//synch and wait other threads
		sync_step_nei();
	}
	n->set_old_routing_tables();
}



void Network::transitive_closure_step(Node *n)
{			
	while (!finished_tc)
	{
		// reset changed tab
		tab_changed->at(n->get_id()) = false;

		// for all routes
		transitive_closure_node(n);

		// synch step 
		sync_step_tc();
	}
}


void Network::transitive_closure_node(Node *n)
{
	// For routes end with a 

	list<Tuple> n_old_buff_a_a = old_buff_tc->at(n->get_id())[0];
	list<Tuple> n_old_buff_a_b = old_buff_tc->at(n->get_id())[1];
	list<Tuple> n_old_buff_b_b = old_buff_tc->at(n->get_id())[2];
	list<Tuple> n_old_buff_b_a = old_buff_tc->at(n->get_id())[3];

	unordered_map<Index,Row> n_table_a_a = n->get_old_routing_table_a_a().get_table();
	unordered_map<Index,Row> n_table_a_b = n->get_old_routing_table_a_b().get_table();
	unordered_map<Index,Row> n_table_b_b = n->get_old_routing_table_b_b().get_table();
	unordered_map<Index,Row> n_table_b_a = n->get_old_routing_table_b_a().get_table();	
	
	// Cartesian product routing table a_a and buffers a_a a_b
	for (unordered_map<Index,Row>::iterator n_tab_a_a_it=n_table_a_a.begin(); n_tab_a_a_it!=n_table_a_a.end(); n_tab_a_a_it++)
	{
		int d_id = n_tab_a_a_it->first.first;

		list<Tuple> d_old_buff_a_a = old_buff_tc->at(d_id)[0];
		for(list<Tuple>::iterator d_buff_a_a_it=d_old_buff_a_a.begin(); d_buff_a_a_it!=d_old_buff_a_a.end(); d_buff_a_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_a_a_it);
			string new_stack = n_tab_a_a_it->first.second;
			int new_cost = n_tab_a_a_it->second.first.first.first.first.first + get<2>(*d_buff_a_a_it);
			int next_hop = n_tab_a_a_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_a_a_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_a_a_it);
			int h_max_1 = n_tab_a_a_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_a_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[0].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{	
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		list<Tuple> d_old_buff_a_b = old_buff_tc->at(d_id)[1];
		for(list<Tuple>::iterator d_buff_a_b_it=d_old_buff_a_b.begin(); d_buff_a_b_it!=d_old_buff_a_b.end(); d_buff_a_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_a_b_it);
			string new_stack = n_tab_a_a_it->first.second;
			int new_cost = n_tab_a_a_it->second.first.first.first.first.first + get<2>(*d_buff_a_b_it);
			int next_hop = n_tab_a_a_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_a_a_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_a_b_it);
			int h_max_1 = n_tab_a_a_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_a_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[1].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}		
	}
	
	// Cartesian product buffer a_a and routing tables a_a a_b
	for(list<Tuple>::iterator n_buff_a_a_it=n_old_buff_a_a.begin(); n_buff_a_a_it!=n_old_buff_a_a.end(); n_buff_a_a_it++)
	{
		int d_id = get<0>(*n_buff_a_a_it);

		unordered_map<Index,Row> d_table_a_a = get_node(d_id)->get_old_routing_table_a_a().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_a_a_it=d_table_a_a.begin(); d_tab_a_a_it!=d_table_a_a.end(); d_tab_a_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_a_a_it->first.first;
			string new_stack = get<1>(*n_buff_a_a_it);
			int new_cost = d_tab_a_a_it->second.first.first.first.first.first + get<2>(*n_buff_a_a_it);
			int next_hop = get<5>(*n_buff_a_a_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_a_a_it);
			char new_x_in = d_tab_a_a_it->second.first.first.second;
			int h_max_1 = d_tab_a_a_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_a_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[0].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		unordered_map<Index,Row> d_table_a_b = get_node(d_id)->get_old_routing_table_a_b().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_a_b_it=d_table_a_b.begin(); d_tab_a_b_it!=d_table_a_b.end(); d_tab_a_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_a_b_it->first.first;
			string new_stack = get<1>(*n_buff_a_a_it);
			int new_cost = d_tab_a_b_it->second.first.first.first.first.first + get<2>(*n_buff_a_a_it);
			int next_hop = get<5>(*n_buff_a_a_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_a_a_it);
			char new_x_in = d_tab_a_b_it->second.first.first.second;
			int h_max_1 = d_tab_a_b_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_a_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[1].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if (tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}
	}

	// Cartesian product routing tables b_a and buffers a_a a_b
	for (unordered_map<Index,Row>::iterator n_tab_b_a_it=n_table_b_a.begin(); n_tab_b_a_it!=n_table_b_a.end(); n_tab_b_a_it++)
	{
		int d_id = n_tab_b_a_it->first.first;

		list<Tuple> d_old_buff_a_a = old_buff_tc->at(d_id)[0];
		for(list<Tuple>::iterator d_buff_a_a_it=d_old_buff_a_a.begin(); d_buff_a_a_it!=d_old_buff_a_a.end(); d_buff_a_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_a_a_it);
			string new_stack = n_tab_b_a_it->first.second;
			int new_cost = n_tab_b_a_it->second.first.first.first.first.first + get<2>(*d_buff_a_a_it);
			int next_hop = n_tab_b_a_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_b_a_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_a_a_it);
			int h_max_1 = n_tab_b_a_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_a_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true) )
			{
				n->get_routing_table_b_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[3].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		list<Tuple> d_old_buff_a_b = old_buff_tc->at(d_id)[1];
		for(list<Tuple>::iterator d_buff_a_b_it=d_old_buff_a_b.begin(); d_buff_a_b_it!=d_old_buff_a_b.end(); d_buff_a_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_a_b_it);
			string new_stack = n_tab_b_a_it->first.second;
			int new_cost = n_tab_b_a_it->second.first.first.first.first.first + get<2>(*d_buff_a_b_it);
			int next_hop = n_tab_b_a_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_b_a_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_a_b_it);
			int h_max_1 = n_tab_b_a_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_a_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_b_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[2].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}		
	}

	// Cartesian product buffer b_a and routing tables a_a a_b
	for(list<Tuple>::iterator n_buff_b_a_it=n_old_buff_b_a.begin(); n_buff_b_a_it!=n_old_buff_b_a.end(); n_buff_b_a_it++)
	{
		int d_id = get<0>(*n_buff_b_a_it);

		unordered_map<Index,Row> d_table_a_a = get_node(d_id)->get_old_routing_table_a_a().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_a_a_it=d_table_a_a.begin(); d_tab_a_a_it!=d_table_a_a.end(); d_tab_a_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_a_a_it->first.first;
			string new_stack = get<1>(*n_buff_b_a_it);
			int new_cost = d_tab_a_a_it->second.first.first.first.first.first + get<2>(*n_buff_b_a_it);
			int next_hop = get<5>(*n_buff_b_a_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_b_a_it);
			char new_x_in = d_tab_a_a_it->second.first.first.second;
			int h_max_1 = d_tab_a_a_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_b_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_b_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[3].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		unordered_map<Index,Row> d_table_a_b = get_node(d_id)->get_old_routing_table_a_b().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_a_b_it=d_table_a_b.begin(); d_tab_a_b_it!=d_table_a_b.end(); d_tab_a_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_a_b_it->first.first;
			string new_stack = get<1>(*n_buff_b_a_it);
			int new_cost = d_tab_a_b_it->second.first.first.first.first.first + get<2>(*n_buff_b_a_it);
			int next_hop = get<5>(*n_buff_b_a_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_b_a_it);
			char new_x_in = d_tab_a_b_it->second.first.first.second;
			int h_max_1 = d_tab_a_b_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_b_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_b_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[2].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}
	}

	// For routes end with b 
	
	// Cartesian product routing table a_b and buffers b_a b_b
	for (unordered_map<Index,Row>::iterator n_tab_a_b_it=n_table_a_b.begin(); n_tab_a_b_it!=n_table_a_b.end(); n_tab_a_b_it++)
	{
		int d_id = n_tab_a_b_it->first.first;

		list<Tuple> d_old_buff_b_a = old_buff_tc->at(d_id)[3];
		for(list<Tuple>::iterator d_buff_b_a_it=d_old_buff_b_a.begin(); d_buff_b_a_it!=d_old_buff_b_a.end(); d_buff_b_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_b_a_it);
			string new_stack = n_tab_a_b_it->first.second;
			int new_cost = n_tab_a_b_it->second.first.first.first.first.first + get<2>(*d_buff_b_a_it);
			int next_hop = n_tab_a_b_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_a_b_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_b_a_it);
			int h_max_1 = n_tab_a_b_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_b_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[0].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		list<Tuple> d_old_buff_b_b = old_buff_tc->at(d_id)[2];
		for(list<Tuple>::iterator d_buff_b_b_it=d_old_buff_b_b.begin(); d_buff_b_b_it!=d_old_buff_b_b.end(); d_buff_b_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_b_b_it);
			string new_stack = n_tab_a_b_it->first.second;
			int new_cost = n_tab_a_b_it->second.first.first.first.first.first + get<2>(*d_buff_b_b_it);
			int next_hop = n_tab_a_b_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_a_b_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_b_b_it);
			int h_max_1 = n_tab_a_b_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_b_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[1].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}		
	}

	// Cartesian product buffer a_b and routing tables b_a b_b
	for(list<Tuple>::iterator n_buff_a_b_it=n_old_buff_a_b.begin(); n_buff_a_b_it!=n_old_buff_a_b.end(); n_buff_a_b_it++)
	{
		int d_id = get<0>(*n_buff_a_b_it);

		unordered_map<Index,Row> d_table_b_a = get_node(d_id)->get_old_routing_table_b_a().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_b_a_it=d_table_b_a.begin(); d_tab_b_a_it!=d_table_b_a.end(); d_tab_b_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_b_a_it->first.first;
			string new_stack = get<1>(*n_buff_a_b_it);
			int new_cost = d_tab_b_a_it->second.first.first.first.first.first + get<2>(*n_buff_a_b_it);
			int next_hop = get<5>(*n_buff_a_b_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_a_b_it);
			char new_x_in = d_tab_b_a_it->second.first.first.second;
			int h_max_1 = d_tab_b_a_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_a_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[0].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		unordered_map<Index,Row> d_table_b_b = get_node(d_id)->get_old_routing_table_b_b().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_b_b_it=d_table_b_b.begin(); d_tab_b_b_it!=d_table_b_b.end(); d_tab_b_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_b_b_it->first.first;
			string new_stack = get<1>(*n_buff_a_b_it);
			int new_cost = d_tab_b_b_it->second.first.first.first.first.first + get<2>(*n_buff_a_b_it);
			int next_hop = get<5>(*n_buff_a_b_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_a_b_it);
			char new_x_in = d_tab_b_b_it->second.first.first.second;
			int h_max_1 = d_tab_b_b_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_a_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_a_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[1].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}
	}

	// Cartesian product routing table b_b and buffers b_a b_b
	for (unordered_map<Index,Row>::iterator n_tab_b_b_it=n_table_b_b.begin(); n_tab_b_b_it!=n_table_b_b.end(); n_tab_b_b_it++)
	{
		int d_id = n_tab_b_b_it->first.first;

		list<Tuple> d_old_buff_b_a = old_buff_tc->at(d_id)[3];
		for(list<Tuple>::iterator d_buff_b_a_it=d_old_buff_b_a.begin(); d_buff_b_a_it!=d_old_buff_b_a.end(); d_buff_b_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_b_a_it);
			string new_stack = n_tab_b_b_it->first.second;
			int new_cost = n_tab_b_b_it->second.first.first.first.first.first + get<2>(*d_buff_b_a_it);
			int next_hop = n_tab_b_b_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_b_b_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_b_a_it);
			int h_max_1 = n_tab_b_b_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_b_a_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_b_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[3].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		list<Tuple> d_old_buff_b_b = old_buff_tc->at(d_id)[2];
		for(list<Tuple>::iterator d_buff_b_b_it=d_old_buff_b_b.begin(); d_buff_b_b_it!=d_old_buff_b_b.end(); d_buff_b_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = get<0>(*d_buff_b_b_it);
			string new_stack = n_tab_b_b_it->first.second;
			int new_cost = n_tab_b_b_it->second.first.first.first.first.first + get<2>(*d_buff_b_b_it);
			int next_hop = n_tab_b_b_it->second.first.first.first.first.second; 
			AdaptationFunction *adapt_func = n_tab_b_b_it->second.first.first.first.second;
			char new_x_in = get<3>(*d_buff_b_b_it);
			int h_max_1 = n_tab_b_b_it->second.first.second;
			int h_max_2 = get<4>(*d_buff_b_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true) )
			{
				n->get_routing_table_b_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[2].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}		
	}

	// Cartesian product buffer b_b and routing tables b_a b_b
	for(list<Tuple>::iterator n_buff_b_b_it=n_old_buff_b_b.begin(); n_buff_b_b_it!=n_old_buff_b_b.end(); n_buff_b_b_it++)
	{
		int d_id = get<0>(*n_buff_b_b_it);

		unordered_map<Index,Row> d_table_b_a = get_node(d_id)->get_old_routing_table_b_a().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_b_a_it=d_table_b_a.begin(); d_tab_b_a_it!=d_table_b_a.end(); d_tab_b_a_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_b_a_it->first.first;
			string new_stack = get<1>(*n_buff_b_b_it);
			int new_cost = d_tab_b_a_it->second.first.first.first.first.first + get<2>(*n_buff_b_b_it);
			int next_hop = get<5>(*n_buff_b_b_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_b_b_it);
			char new_x_in = d_tab_b_a_it->second.first.first.second;
			int h_max_1 = d_tab_b_a_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_b_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true) )
			{
				n->get_routing_table_b_a()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[3].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}

		unordered_map<Index,Row> d_table_b_b = get_node(d_id)->get_old_routing_table_b_b().get_table();
		for (unordered_map<Index,Row>::iterator d_tab_b_b_it=d_table_b_b.begin(); d_tab_b_b_it!=d_table_b_b.end(); d_tab_b_b_it++)
		{
			n->increment_nb_opers_tc();
			int new_dest = d_tab_b_b_it->first.first;
			string new_stack = get<1>(*n_buff_b_b_it);
			int new_cost = d_tab_b_b_it->second.first.first.first.first.first + get<2>(*n_buff_b_b_it);
			int next_hop = get<5>(*n_buff_b_b_it); 
			AdaptationFunction *adapt_func = get<6>(*n_buff_b_b_it);
			char new_x_in = d_tab_b_b_it->second.first.first.second;
			int h_max_1 = d_tab_b_b_it->second.first.second;
			int h_max_2 = get<4>(*n_buff_b_b_it);
			int h_max = h_max_1; 
			if (h_max_2 > h_max)
			{
				h_max = h_max_2;
			}
			bool changed = n->get_routing_table()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
			if((changed == true))
			{
				n->get_routing_table_b_b()->add_route(new_dest, new_stack, new_cost, next_hop, adapt_func, new_x_in, h_max, nb_iters_tc);
				new_buff_tc->at(n->get_id())[2].push_front(make_tuple(new_dest, new_stack, new_cost, new_x_in, h_max, next_hop, adapt_func));
				if(tab_changed->at(n->get_id()) == false)
				{
					tab_changed->at(n->get_id()) = true;
				}
			}
		}
	}

}


void Network::start_node(Node *n)
{
	// initialization step
	init_step(n);
	
	// synchronization step
	sync_step_nei();
	
	// BF step 
	neighborhood_step(n);

	// FW step
	transitive_closure_step(n);
	
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
	//add nb_iters_nei_tc
	nb_iters = nb_iters_nei +  nb_iters_tc - 1;

	//add nb_opers_nei_tc
	for (auto n : nodes)
	{
		total_nb_opers_nei += n->get_nb_opers_nei();
		total_nb_opers_tc += n->get_nb_opers_tc();
	}
}


void Network::update_finished_nei()
{
    finished_nei = all_of(tab_changed->begin(), tab_changed->end(), [](bool elt){return (elt==false);});
}


void Network::update_finished_tc()
{
    finished_tc = all_of(tab_changed->begin(), tab_changed->end(), [](bool elt){return (elt==false);});
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


void Network::update_buffers_tc()
{
	tmp_buff_tc = old_buff_tc;
	old_buff_tc = new_buff_tc;
	new_buff_tc = tmp_buff_tc;

	for (vector<array<list<Tuple>,4>>::iterator v_iter = new_buff_tc->begin(); v_iter != new_buff_tc->end(); v_iter++)
	{
		(*v_iter)[0].clear();
		(*v_iter)[1].clear();	
		(*v_iter)[2].clear();	
		(*v_iter)[3].clear();	
	}
}

void Network::update_routing_tables()
{
	for(auto n : nodes)
	{
		n->set_old_routing_tables();
	}
}

int Network::get_number_iters()
{
	nb_iters = nb_iters_nei + nb_iters_tc - 1;
	return nb_iters;
}

int Network::get_number_iters_nei()
{
	return nb_iters_nei;
}

int Network::get_number_iters_tc()
{
	return nb_iters_tc - 1;
}


long long int Network::get_nb_opers_nei()
{
	return total_nb_opers_nei;
}

long long int Network::get_nb_opers_tc()
{
	return total_nb_opers_tc;
}

long long int * Network::get_number_unfeasible_routes()
{
	for(auto n : nodes)
	{	
		unordered_map<Index,Row> table = n->get_routing_table()->get_table();
		for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); it++)
  		{
  			string protocol_stack = it->first.second; 
			int h_max = it->second.first.second; 
  			if ((protocol_stack.size() != 1))
  			{  
				number_unfeasible_routes[h_max-1] += 1;
			}
		}
	}
	return number_unfeasible_routes;
	
}

long long int * Network::get_number_feasible_routes()
{
	for(auto n : nodes)
	{	
		unordered_map<Index,Row> table = n->get_routing_table()->get_table();
		for (unordered_map<Index,Row>::iterator it=table.begin(); it!=table.end(); it++)
  		{
  			string protocol_stack = it->first.second; 
			int h_max = it->second.first.second; 
  			if ((protocol_stack.size() == 1))
  			{  
				number_feasible_routes[h_max-1] += 1;
			}
		}
	}
	return number_feasible_routes;	
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

	delete old_buff_tc;
	delete new_buff_tc; 

	delete tab_threads; 
	delete tab_changed;
}