#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <chrono>
#include "TopologyParser.hpp"


class Simulation
{
	private: 
		Network *network;
		TopologyParser *topo_parser; 
		chrono::steady_clock::time_point start_time, end_time;
		float convergence_time_s;
	
	public: 
		Simulation(char* in_file, int h_max, int metric_cv, int metric_ec);		
		void set_network(Network *_network);
		Network* get_network();
		float get_convergence_time();
		long long int get_nb_opers_nei();
		long long int get_nb_opers_tc();
		int get_number_iters();
		int get_number_iters_nei();
		int get_number_iters_tc();
		float * get_nb_seg();
		long long int * get_number_unfeasible_routes();
		void start();
		void stop();
		~Simulation();
};

#endif