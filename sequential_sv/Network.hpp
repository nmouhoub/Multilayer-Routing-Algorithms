#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <limits>
#include <vector>
#include <list>
#include <array>
#include <set>
#include <algorithm>
#include "Node.hpp"
#include "Link.hpp"

using namespace std;

typedef tuple<int,string,int,int,int> Tuple;

class Network 
{
	private :    
        int max_stack;
        bool finished;
        int nb_iters;
		unordered_set<Node*> nodes;
		unordered_set<Link*> links;
		unordered_set<char> protocols; 
		unordered_set<AdaptationFunction *> adapt_functions; 
		vector<array<list<Tuple>,2>> *old_buff;
		vector<array<list<Tuple>,2>> *new_buff;
		vector<array<list<Tuple>,2>> *tmp_buff;
		
	public : 
		Network(int nb_nodes, unordered_set<char> _protocols, int _max_stack);
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
		void init(); 
		void start();
		void update_buffers();
		int get_number_iters();
		~Network();
};	

#endif