#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <set>
#include <array>
#include <algorithm>
#include "Node.hpp"
#include "Link.hpp"

using namespace std;


class Network 
{
	private :    
        int max_stack;
		long long int total_nb_message;
		unordered_set<Node*> nodes;
		unordered_set<Link*> links;
		unordered_set<char> protocols; 
		unordered_set<AdaptationFunction *> adapt_functions; 
		
	public : 
		Network(unordered_set<char> _protocols, int _max_stack);
		int get_max_stack();
		long long int get_nb_message();
		unordered_set<Node*> get_nodes();
		unordered_set<Link*> get_links();
	    Node *get_node(int _id);
		unordered_set<char> get_protocols();
		void set_nodes(unordered_set<Node*> _nodes);
		void set_links(unordered_set<Link*> _links);
		unordered_set<AdaptationFunction*> get_adapt_functions();
		void set_adapt_functions(unordered_set<AdaptationFunction*> _adapt_functions);
		void start();
		void wait();
		void stop();
		~Network();
};	

#endif