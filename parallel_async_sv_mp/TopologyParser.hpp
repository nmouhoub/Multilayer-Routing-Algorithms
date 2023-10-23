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
        void configure_threads(Node* src, Node* dest, Link* link);
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