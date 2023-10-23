#ifndef SIMULATION_HPP
#define SIMULATION_HPP

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
		long long int get_nb_message();
		void start();
		void wait();
		void stop();
		~Simulation();
};

#endif