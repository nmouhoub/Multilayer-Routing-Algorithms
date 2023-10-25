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

#ifndef NODE_HPP
#define NODE_HPP

#include <unordered_set>

#include "Link.hpp"
#include "Message.hpp"
#include "RoutingTable.hpp"
#include "AdaptationFunction.hpp"
#include "ConcurrentQueue.hpp"


using namespace std;

typedef unordered_map<int, Link*>::iterator LinkIter;
typedef unordered_map<int, ConcurrentQueue<int>*>::iterator QueueIter;

class Node 
{
	private : 
    	int id;
    	bool finished;
    	int max_stack;
		int nb_message;
	    unordered_set<int> neighbors;
	    unordered_set<char> protocols_in;  						
	    unordered_set<char> protocols_out;
	    RoutingTable *routing_table;
	    unordered_set<AdaptationFunction *> adapt_functions;
	    thread m_thread;
	    unordered_map<int, ConcurrentQueue<int>*> neighbors_queue_id;
	    unordered_map<int, Link*> links_out;
	    ConcurrentQueue<int>* queue_id;
	    unordered_map<int, Link*> links_in;				

	public : 
		Node(int _id, int _max_stack);
		void add_link_in(Link *link);
		void add_link_out(Link *link);
		void add_neighbor(int _neighbor);
		void set_neighbors_id(unordered_set<int> _neighbors);
		void add_adapt_func(AdaptationFunction *adapt_func);
		void add_neighbor_queue(int _id, ConcurrentQueue<int>* _neighbor_queue);
		void set_adapt_functions(unordered_set<AdaptationFunction*> _adapt_functions);
		int get_id();
		bool get_status();
		int get_max_stack();
		int get_nb_message();
		unordered_set<int> get_neighbors_id();
	    unordered_set<char> get_protocols_in();
	    unordered_set<char> get_protocols_out();
	    ConcurrentQueue<int>* get_queue();
	    RoutingTable* get_routing_table();
	    unordered_set<AdaptationFunction *> get_adapt_functions();
	    void start();
	    void stop();
	   	void init();
	    void init_routing_table();
	    void wait_for_message();
	    void send(int receiver, Message *msg);
	    void receive(int sender, Message *msg);	
	    ~Node();
};

#endif