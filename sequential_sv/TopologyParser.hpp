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

#ifndef TOPOLOGY_PARSER_HPP
#define TOPOLOGY_PARSER_HPP

#include <string.h>
#include <sstream>
#include <fstream>
#include "Network.hpp"

using namespace std;

const int default_cost = 1;
const string all_protocols = "abcdefghijklmnopqrstuvwxyz";


class TopologyParser 
{

    public:
        TopologyParser();
        unordered_set<char> generate_valid_protocols(int nb_protocols);
        Network *generate_topology_from_file(char *in_file, int h_max, int metric_cv, int metric_ec);
        void generate_nodes_from_file(ifstream& topo_file, Network* network, int nb_nodes);
        void generate_links_from_file(ifstream& topo_file, Network* network, int metric_cv, int metric_ec);
        bool is_new_link(string line);
        AdaptationFunction* exists_adapt_func(unordered_set<AdaptationFunction*>& adapt_functions, 
                                                          int type, char from, char to);
        ~TopologyParser();
};


#endif