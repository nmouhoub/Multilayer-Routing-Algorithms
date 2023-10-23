#ifndef NODE_HPP
#define NODE_HPP

#include <unordered_set>
#include "Link.hpp"
#include "RoutingTable.hpp"
#include "AdaptationFunction.hpp"


using namespace std;

class Node 
{
	private : 
    	int id;
	    unordered_set<int> neighbors;
	    unordered_set<int> neighbors_pred;
	    unordered_set<int> neighbors_succ;
	    unordered_set<char> protocols_in;  						
	    unordered_set<char> protocols_out;
	    RoutingTable *routing_table;
	    unordered_set<AdaptationFunction *> adapt_functions;	

	public : 
		Node(int _id);
		void init_routing_table();
		void add_neighbor(int _neighbor);
		void add_neighbor_pred(int _neighbor);
		void add_neighbor_succ(int _neighbor);
		void set_neighbors_id(unordered_set<int> _neighbors);
		void add_adapt_func(AdaptationFunction *adapt_func);
		void set_adapt_functions(unordered_set<AdaptationFunction*> _adapt_functions);
		int get_id();
		bool get_status();
		int get_max_stack();
		unordered_set<int> get_neighbors_id();
		unordered_set<int> get_neighbors_pred_id();
		unordered_set<int> get_neighbors_succ_id();
	    unordered_set<char> get_protocols_in();
	    unordered_set<char> get_protocols_out();
	    RoutingTable* get_routing_table();
	    unordered_set<AdaptationFunction *> get_adapt_functions();
	    ~Node();
};

#endif