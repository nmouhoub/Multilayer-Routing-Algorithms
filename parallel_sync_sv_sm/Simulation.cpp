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

long long int Simulation::get_number_opers()
{
	return network->get_number_opers();
}

float Simulation::get_number_paths()
{
	return network->get_number_paths();
}

int * Simulation::get_net_diameter()
{
	return network->get_net_diameter();
}


float * Simulation::get_nb_hops()
{
	return network->get_number_hops();
}

float * Simulation::get_nb_costs()
{
	return network->get_nb_costs();
}

int Simulation::get_nb_nodes()
{
	return network->get_nb_nodes();
}

int Simulation::get_nb_links()
{
	return network->get_nb_links();
}

long long int * Simulation::get_number_feasible_routes()
{
	return network->get_number_feasible_routes();
}

long long int * Simulation::get_number_unfeasible_routes()
{
	return network->get_number_unfeasible_routes();
}

long long int Simulation::get_number_found_routes()
{
	return network->get_number_found_routes();
}

void Simulation::start()
{
	start_time = chrono::steady_clock::now();
 	network->start(); 
}

void Simulation::stop()
{
	end_time = chrono::steady_clock::now();
}

Simulation::~Simulation()
{
	delete topo_parser;
	delete network;
}