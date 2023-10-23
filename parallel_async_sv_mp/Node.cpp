#include "Node.hpp"

Node::Node(int _id, int _max_stack)
{
	id = _id;
	finished = false;
	max_stack = _max_stack;
	queue_id = new ConcurrentQueue<int>();
	routing_table = new RoutingTable();
	nb_message = 0;
}

void Node::add_neighbor(int _neighbor)
{
	neighbors.insert(_neighbor);
}

void Node::add_link_in(Link *link)
{
	links_in.insert(make_pair(link->get_src(), link));
}

void Node::add_link_out(Link *link)
{
	links_out.insert(make_pair(link->get_dest(), link));
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

void Node::add_neighbor_queue(int _id, ConcurrentQueue<int>* _neighbor_queue)
{
	neighbors_queue_id.insert(make_pair(_id, _neighbor_queue));
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

bool Node::get_status()
{ 
	return finished;
}

int Node::get_max_stack()
{
	return max_stack;
}

int Node::get_nb_message()
{
	return nb_message;
}

unordered_set<int> Node::get_neighbors_id()
{
	return neighbors;
}

unordered_set<char> Node::get_protocols_in()
{
	return protocols_in;
}

unordered_set<char> Node::get_protocols_out()
{
	return protocols_out;
}

ConcurrentQueue<int>* Node::get_queue()
{
	return queue_id;
}

RoutingTable* Node::get_routing_table()
{
	return routing_table;
}

unordered_set<AdaptationFunction *> Node::get_adapt_functions()
{
	return adapt_functions;
}

void Node::start()
{
	//init_routing_table();
	m_thread = thread(&Node::init, this);
}

void Node::stop()
{
	finished = true;
}

void Node::init_routing_table()
{
	for(auto f : adapt_functions)
	{	
		routing_table->add_route(id, string(1,f->get_from()), 0, id, f);
	}
}

void Node::init()
{
	for(auto x: protocols_in) 
	{
		for(auto n_id : neighbors)
		{
			send(n_id ,new Message(id, string(1,x), 0));
			nb_message += 1;
		}
	}
	wait_for_message();
}

void Node::wait_for_message()
{
	while(!finished)
	{	 
		int sender = -1; 
		queue_id->pop(sender);
		if(sender != -1)
		{
			LinkIter link_iter = links_in.find(sender);
	        if (link_iter != links_in.end())
	        {
	        	Link *link = link_iter->second;
	    	    Message *msg = link->get_message();
	    	    receive(sender, msg);
	        }
		}
	}
}


void Node::send(int receiver, Message *msg)
{
	LinkIter link_iter = links_out.find(receiver);
	QueueIter queue_iter = neighbors_queue_id.find(receiver);
    if ( (link_iter != links_out.end()) && (queue_iter != neighbors_queue_id.end()) )
    {
	    Link *link = link_iter->second;
	    ConcurrentQueue<int> *queue_receiver = queue_iter->second;
	    link->set_message(msg);
	    queue_receiver->push(id); 
    }
}

void Node::receive(int sender, Message *msg)
{		
    for(auto func : adapt_functions)
    {    	
    	AdaptationFunction *rev_func = func->reverse();
		string new_stack = msg->get_stack();
		if(rev_func->apply_to_stack(new_stack))
		{
			if( (!new_stack.empty()) && ((int) new_stack.size() <= max_stack) )
			{
				LinkIter link_iter = links_out.find(sender);
	    		if (link_iter != links_out.end())
	    		{
	    			Link *link = link_iter->second;
					int new_cost = msg->get_cost() + link->get_cost(func);
					if(routing_table->add_route(msg->get_dest(), new_stack, new_cost, sender, func))
					{
						for(auto n_id : neighbors)
						{ 
							send(n_id, new Message(msg->get_dest(), new_stack, new_cost));
							nb_message += 1;
						}
					}
				}
			}
		}
		delete rev_func;
    }
    delete msg;
}

Node::~Node()
{
	if(m_thread.joinable()) m_thread.join();
	delete routing_table;
	delete queue_id;
}