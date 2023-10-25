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

#ifndef NETWORK_HPP
#define NETWORK_HPP


#include <thread>
#include <set>
#include <vector>
#include <list>
#include <array>
#include <algorithm>
#include <atomic>
#include <memory>
#include <math.h>
#include "Node.hpp"
#include "Link.hpp"
#include <mutex>
#include <condition_variable>

using namespace std;

typedef tuple<int,string,int,int,int> Tuple;

class Network 
{
	private :    
		int max_stack;
        bool finished;
        int nb_iters;
		long long int * number_feasible_routes;
		long long int * number_unfeasible_routes;
		int nb_nodes, nb_links;
		unordered_set<Node*> nodes;
		unordered_set<Link*> links;
		unordered_set<char> protocols; 
		unordered_set<AdaptationFunction *> adapt_functions; 
		vector<array<list<Tuple>,2>> *old_buff;
		vector<array<list<Tuple>,2>> *new_buff;
		vector<array<list<Tuple>,2>> *tmp_buff;
		vector<bool> *tab_changed;
		vector<thread> *tab_threads;
		mutex mtx;
    	condition_variable cv;
    	unsigned int m_count;
  		unsigned int m_generation;
  		unsigned int m_count_reset_value;
		int * net_diameter;  
		float * nb_hops; 
		float * nb_costs;
		long long int total_nb_found_routes;

	public : 
		Network(int _nb_nodes, int _nb_links, unordered_set<char> _protocols, int _max_stack);
		int get_max_stack();
		unordered_set<Node*> get_nodes();
		unordered_set<Link*> get_links();
	    Node *get_node(int _id);
		unordered_set<char> get_protocols();
		Link *get_link(int src, int dest);
		void set_nodes(unordered_set<Node*> _nodes);
		void set_links(unordered_set<Link*> _links);
		unordered_set<AdaptationFunction*> get_adapt_functions();
		void set_adapt_functions(unordered_set<AdaptationFunction*> _adapt_functions);
		void init_step(Node *n);
		void sync_step();
		void main_step(Node *n);
		void start();
		void start_node(Node *n);
		void update_buffers();
		void update_finished();
		float * get_number_hops();
		float get_number_paths();
		int get_number_iters();
		long long int get_number_opers();
		int * get_net_diameter();
		float * get_nb_costs();
		int get_nb_nodes();
		int get_nb_links();
		long long int get_number_found_routes();
		long long int * get_number_feasible_routes();
		long long int * get_number_unfeasible_routes();
		~Network();
};	

#endif