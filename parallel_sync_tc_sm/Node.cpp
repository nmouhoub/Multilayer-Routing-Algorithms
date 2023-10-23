#include "Node.hpp"

Node::Node(int _id)
{
	id = _id;
	nb_opers_nei = 0;
	nb_opers_tc = 0;
	routing_table = new RoutingTable();
	final_routing_table[0] = new RoutingTable();
	final_routing_table[1] = new RoutingTable();
	final_routing_table[2] = new RoutingTable();
	final_routing_table[3] = new RoutingTable();
}

void Node::add_neighbor(int _neighbor)
{
	neighbors.insert(_neighbor);
}

void Node::add_neighbor_pred(int _neighbor)
{
	neighbors_pred.insert(_neighbor);
}

void Node::add_neighbor_succ(int _neighbor)
{
	neighbors_succ.insert(_neighbor);
}

void Node::set_neighbors_id(unordered_set<int> _neighbors)
{
	neighbors = _neighbors;
}

void Node::add_adapt_func(AdaptationFunction *adapt_func)
{
	adapt_functions.insert(adapt_func);
	protocols_in.insert(adapt_func->get_from());
	protocols_out.insert(adapt_func->get_to());
}

void Node::set_adapt_functions(unordered_set<AdaptationFunction*> _adapt_functions)
{
	adapt_functions = _adapt_functions;
	for(auto f : adapt_functions)
	{
		if(f->get_type() == DC)
		{
			protocols_in.insert(f->get_to());
			protocols_out.insert(f->get_from());
		}
		protocols_in.insert(f->get_from());
		protocols_out.insert(f->get_to());	
	}
}

int Node::get_id()
{
	return id;
}

unordered_set<int> Node::get_neighbors_id()
{
	return neighbors;
}

unordered_set<int> Node::get_neighbors_pred_id()
{
	return neighbors_pred;
}

unordered_set<int> Node::get_neighbors_succ_id()
{
	return neighbors_succ;
}

unordered_set<char> Node::get_protocols_in()
{
	return protocols_in;
}

unordered_set<char> Node::get_protocols_out()
{
	return protocols_out;
}

RoutingTable* Node::get_routing_table()
{
	return routing_table;
}

unordered_set<AdaptationFunction *> Node::get_adapt_functions()
{
	return adapt_functions;
}

void Node::init_routing_table()
{
	for(auto f : adapt_functions)
	{
		if (f->get_from() == 'a')
		{
			final_routing_table[0]->add_route(id, string(1,f->get_from()), 0, id, f, 'a', 1, 0);
		}else
		{
			final_routing_table[1]->add_route(id, string(1,f->get_from()), 0, id, f, 'b', 1, 0);
		}
	}
}



RoutingTable* Node::get_routing_table_a_a()
{
	return final_routing_table[0];
}

RoutingTable* Node::get_routing_table_a_b()
{
	return final_routing_table[1];
}

RoutingTable* Node::get_routing_table_b_b()
{
	return final_routing_table[2];
}

RoutingTable* Node::get_routing_table_b_a()
{
	return final_routing_table[3];
}

RoutingTable Node::get_old_routing_table_a_a()
{
	return final_old_routing_table[0];
}

RoutingTable Node::get_old_routing_table_a_b()
{
	return final_old_routing_table[1];
}

RoutingTable Node::get_old_routing_table_b_b()
{
	return final_old_routing_table[2];
}

RoutingTable Node::get_old_routing_table_b_a()
{
	return final_old_routing_table[3];
}

void Node::set_old_routing_tables()
{
	final_old_routing_table[0] = *final_routing_table[0];
	final_old_routing_table[1] = *final_routing_table[1];
	final_old_routing_table[2] = *final_routing_table[2];
	final_old_routing_table[3] = *final_routing_table[3];
}

int Node::get_nb_opers_nei()
{
	return nb_opers_nei;
}

int Node::get_nb_opers_tc()
{
	return nb_opers_tc;
}

void Node::increment_nb_opers_nei()
{
	nb_opers_nei += 1;
}

void Node::increment_nb_opers_tc()
{
	nb_opers_tc += 1;
}

Node::~Node()
{
	delete routing_table;
}