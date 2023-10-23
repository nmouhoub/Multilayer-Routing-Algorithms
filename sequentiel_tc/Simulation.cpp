#include "Simulation.hpp"


Simulation::Simulation(char *in_file, int h_max, int metric_cv, int metric_ec)
{
  topo_parser = new TopologyParser();
  network = topo_parser->generate_topology_from_file(in_file, h_max, metric_cv, metric_ec);
}

void Simulation::set_network(Network *_network)
{
	network = _network;
}

Network* Simulation::get_network()
{
	return network;
}

float Simulation::get_convergence_time()
{
	convergence_time_s = (float)chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count()/1000;
	return convergence_time_s;
}

int Simulation::get_number_iters()
{
	return network->get_number_iters();
}


void Simulation::display_all_routing_tables()
{
	network->display_all_routing_tables();
}

void Simulation::start()
{
	// Take start time 
	start_time = chrono::steady_clock::now();
 	network->start(); 
}

void Simulation::stop()
{
	// Take end time 
	end_time = chrono::steady_clock::now();
}

Simulation::~Simulation()
{
	delete topo_parser;
	delete network;
}