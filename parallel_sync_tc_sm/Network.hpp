#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <thread>
#include <vector>
#include <list>
#include <array>
#include <algorithm>
#include <atomic>
#include <memory>
#include "Node.hpp"
#include "Link.hpp"
#include <mutex>
#include <condition_variable>

using namespace std;

typedef tuple<int,string,int,char,int,int,AdaptationFunction*> Tuple;

class Network 
{
	private :    
        int max_stack;
        bool finished_nei;
        bool finished_tc;
		int nb_iters;
        int nb_iters_nei;
        int nb_iters_tc;
		long long int total_nb_opers_nei;
		long long int total_nb_opers_tc;
		long long int * number_unfeasible_routes; 
		long long int * number_feasible_routes; 
		unordered_set<Node*> nodes;
		unordered_set<Link*> links;
		unordered_set<char> protocols; 
		unordered_set<AdaptationFunction *> adapt_functions; 
		vector<array<list<Tuple>,2>> *old_buff;
		vector<array<list<Tuple>,2>> *new_buff;
		vector<array<list<Tuple>,2>> *tmp_buff;
		vector<array<list<Tuple>,4>> *old_buff_tc;
		vector<array<list<Tuple>,4>> *new_buff_tc;
		vector<array<list<Tuple>,4>> *tmp_buff_tc;
		vector<bool> *tab_changed;
		vector<thread> *tab_threads;
		mutex mtx;
    	condition_variable cv;
    	unsigned int m_count;
  		unsigned int m_generation;
  		unsigned int m_count_reset_value;
		
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
		void start();
		void start_node(Node *n);
		void update_buffers();
		void update_buffers_tc();
		void update_routing_tables();
		void update_finished_nei();
		void update_finished_tc();
		void init_step(Node *n); 
		void neighborhood_step(Node *n);
		void transitive_closure_step(Node *n);
		void transitive_closure_node(Node *n);
		void sync_step_nei();
		void sync_step_tc();
		int get_number_iters();
		int get_number_iters_nei();
		int get_number_iters_tc();
		long long int get_nb_opers_nei();
		long long int get_nb_opers_tc();
		long long int * get_number_unfeasible_routes();
		long long int * get_number_feasible_routes();
		~Network();
};	

#endif