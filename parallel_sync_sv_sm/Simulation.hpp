/*
 *   Copyright (c) 2023 Noureddine Mouhoub

 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.

 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Lesser General Public License for more details.

 *   You should have received a copy of the GNU Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

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
		int get_number_iters();
		long long int get_number_opers();
		float get_number_paths();
		int * get_net_diameter();
		float * get_nb_hops();
		float * get_nb_costs();
		int get_nb_nodes();
		int get_nb_links();
		long long int * get_number_feasible_routes();
		long long int * get_number_unfeasible_routes();
		long long int get_number_found_routes();
		void display_all_routing_tables();
		void start();
		void stop();
		~Simulation();
};

#endif