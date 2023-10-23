#ifndef LINK_HPP
#define LINK_HPP


#include <utility> 
#include <unordered_map>
#include "AdaptationFunction.hpp"

using namespace std;

typedef unordered_map<AdaptationFunction*, int> MapCost;


class Link
{
	private : 
		int src; 
		int dest; 
		MapCost map_cost;
		
	public :
		Link(int _src, int _dest);
		int get_src();
		int get_dest();	
		MapCost get_map_cost();
		void set_map_cost(MapCost _map_cost);
		int get_cost(AdaptationFunction *adapt_func);
};


#endif