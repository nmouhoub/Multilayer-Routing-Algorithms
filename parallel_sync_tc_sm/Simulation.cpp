#include "Simulation.hpp"

Simulation::Simulation(char *in_file, int h_max, int metric_cv, int metric_ec)
{
  topo_parser = new TopologyParser();
  network = topo_parser->generate_topology_from_file(in_file, h_max, metric_cv, metric_ec);
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

long long int Simulation::get_nb_opers_nei()
{
	return network->get_nb_opers_nei();
}

long long int Simulation::get_nb_opers_tc()
{
	return network->get_nb_opers_tc();
}

int Simulation::get_number_iters()
{
	return network->get_number_iters();
}

int Simulation::get_number_iters_nei()
{
	return network->get_number_iters_nei();
}

int Simulation::get_number_iters_tc()
{
	return network->get_number_iters_tc();
}

long long int * Simulation::get_number_unfeasible_routes()
{
	return network->get_number_unfeasible_routes();
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