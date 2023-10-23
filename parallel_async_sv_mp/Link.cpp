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

bool Link::is_empty()
{
	return messages_queue.empty();
}

void Link::set_message(Message *msg)
{
	messages_queue.push(msg);
}

Message* Link::get_message()
{
	Message *msg = messages_queue.pop();
	return msg;
}