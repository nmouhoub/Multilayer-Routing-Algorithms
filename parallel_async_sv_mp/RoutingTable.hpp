#ifndef ROUTING_TABLE_HPP
#define ROUTING_TABLE_HPP

#include <unordered_map>
#include "AdaptationFunction.hpp"

typedef pair<int,string> Index;
typedef pair<pair<int,int>,AdaptationFunction*> Row;



namespace std {
	template <> struct hash<std::pair<int, string>> {
	    inline size_t operator()(const std::pair<int, string> &v) const {
	        std::hash<int> int_hasher;
	        std::hash<string> string_hasher;
	        return int_hasher(v.first) ^ string_hasher(v.second);
	    }
	};
}

class RoutingTable
{
	private: 
		unordered_map<Index,Row> table; 
	public:
		RoutingTable();

		unordered_map<Index,Row> get_table();
		int get_cost(int dest, string protocol_stack);
		int get_next_hop(int dest, string protocol_stack);
		AdaptationFunction *get_adapt_func(int dest, string protocol_stack);

		bool add_route(int dest, string protocol_stack, int cost, int next_hop, AdaptationFunction *adapt_func);
};

#endif