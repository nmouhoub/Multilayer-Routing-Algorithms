#include "Link.hpp"


Link::Link(int _src, int _dest)
{
	src = _src;
	dest = _dest;
}

int Link::get_src()
{
	return src;
}

int Link::get_dest()
{
	return dest;
}

MapCost Link::get_map_cost()
{
	return map_cost;
}

void Link::set_map_cost(MapCost _map_cost)
{
	map_cost = _map_cost;
}

int Link::get_cost(AdaptationFunction *adapt_func)
{
	return map_cost.find(adapt_func)->second;
}