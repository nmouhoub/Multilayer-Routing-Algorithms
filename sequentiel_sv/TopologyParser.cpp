#include "TopologyParser.hpp"


TopologyParser::TopologyParser() {}


unordered_set<char> TopologyParser::generate_valid_protocols(int nb_protocols)
{
    string valid_protocols = all_protocols.substr(0,nb_protocols);
    unordered_set<char> protocols;
    for(auto p : valid_protocols) {
        protocols.insert(p);
    }
    return protocols;
}

Network *TopologyParser::generate_topology_from_file(char *in_file, int h_max, int metric_cv, int metric_ec)
{   
    ifstream topo_file;
    topo_file.open(in_file);

    topo_file.ignore(50,'\n');
    topo_file.ignore(50,'\n');
    topo_file.ignore(50,'\n');
    topo_file.ignore(50,'\n');
    topo_file.ignore(19,'\n');

    if(!topo_file) 
    { 
       cerr <<"Connot open input file !" << endl; 
    }
    else
    {
        int nb_nodes, nb_links , nb_protocols = 0;
        string line;
        if (getline(topo_file, line))
        {
            istringstream iss(line);
            if (!(iss >> nb_nodes >> nb_links >> nb_protocols)) 
            { 
                cerr << "Error to parse parameters !" << endl; 
                return nullptr;
            } 
        }

        Network *network = new Network(nb_nodes, generate_valid_protocols(nb_protocols), h_max);
        generate_nodes_from_file(topo_file, network, nb_nodes);
        generate_links_from_file(topo_file, network, metric_cv, metric_ec);
        return network;
    }
    topo_file.close();
    return nullptr;
}

void TopologyParser::generate_nodes_from_file(ifstream& topo_file, Network* network, int nb_nodes)
{
    string line;
    unordered_set<Node*> nodes; 
    topo_file.ignore(19,'\n');
    for (int i = 0 ; i < nb_nodes ; i++)
    {
        if(!getline(topo_file, line))
        {
            cerr << "Error to parse nodes !" << endl;
        }
        nodes.insert(new Node(stoi(line)));
    }
    topo_file.ignore(19,'\n');
    network->set_nodes(nodes);
}

void TopologyParser::generate_links_from_file(ifstream& topo_file, Network* network, int metric_cv, int metric_ec)
{
    MapCost map_cost;
    unordered_set<Link*> links;
    unordered_set<AdaptationFunction*> adapt_functions_node;
    unordered_set<AdaptationFunction*> adapt_functions_network;

    string line;
    int src, dest, cost; 
    int type; char from, to, c;

    while ( getline(topo_file,line) )
    {
        if(is_new_link(line)) 
        {
            Link *link = new Link(src, dest);
            link->set_map_cost(map_cost);
            links.insert(link);
            network->get_node(src)->add_neighbor(dest);
            network->get_node(dest)->add_neighbor(src);
            network->get_node(src)->add_neighbor_succ(dest);
            network->get_node(dest)->add_neighbor_pred(src);
            map_cost = MapCost();
        }
        else 
        {
            istringstream iss(line);
            if (!(iss >> c >> src >> c >> c >> dest >> c >> type >> from >> to >> cost )) 
            { 
                cerr << "Error in parse links !" << endl; 
            } 
            else
            {

                if ((type == 0) && (from != to)) 
                {
                    cost += metric_cv;  
                }
                else if ( (type != 0) )
                {
                    cost += metric_ec;
                }

                AdaptationFunction* func = exists_adapt_func(adapt_functions_network, type, from, to);
                network->get_node(src)->add_adapt_func(func);
                map_cost.insert(make_pair(func, cost));
            }
        }  
    }
    network->set_links(links);
    network->set_adapt_functions(adapt_functions_network);
}

bool TopologyParser::is_new_link(string line)
{
    if(line.compare("==================") == 0)
        return true; 
    return false;
}

AdaptationFunction* TopologyParser::exists_adapt_func(unordered_set<AdaptationFunction*>& adapt_functions, int type, char from, char to)
{
    unordered_set<AdaptationFunction*>::iterator it;
    for (it=adapt_functions.begin(); it!=adapt_functions.end(); ++it)
    {
        if( ((*it)->get_type() == type) && ((*it)->get_from()== from) && ((*it)->get_to()== to) )
        {
            return (*it);
        }
    }
    AdaptationFunction *adapt_func = new AdaptationFunction(type, from, to);
    adapt_functions.insert(adapt_func);
    return adapt_func;
}

TopologyParser::~TopologyParser(){}