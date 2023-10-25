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

#include <iomanip>
#include <getopt.h> 
#include <string.h>
#include <cstring>
#include <stdlib.h>
#include "Simulation.hpp"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

using namespace std;

void usage(char *program_name, int status) {
    if (status == EXIT_SUCCESS)
    {
    	cout << "Usage: " << program_name << " -i Topologies/topo.txt -s 3 -c 1 -e 1" << endl;
	    cout << "    -i: input_file topology (ex:Topologies/topo.txt)" << endl
	         << "    -s: maximum protocol stack height (non negative)" << endl
	         << "    -c: conversion cost (non negative)" << endl
	         << "    -e: encapsulation cost (non negative)" << endl
	         << "    -h: help" << endl;
    }
    else
    {
      cerr << "Try '" << program_name << " -h' for help." << endl;
    }
  	exit(status);
}


void parse_args(int argc, char *argv[], char* &in_file, int& max_stack, int& cost_cv, int& cost_ec)
{
	extern char *optarg;
	extern int optopt;
  	char c;

  	while ((c = getopt(argc, argv, "i:s:c:e:h")) != EOF)
  	{
    	switch (c) 
    	{
	    	case 'h':       /* help */
	      		usage(argv[0], EXIT_SUCCESS);
	      		break;
			case 'i':  		/* input file */
				in_file = optarg;	
				break;
	     	case 's':		/* maximum stack */
		    	max_stack = atoi(optarg);
		    	break;
		    case 'c':		/* cost of conversion */
		    	cost_cv = atoi(optarg);
		    	break;
			case 'e':		/* cost of encapsulation */
		    	cost_ec = atoi(optarg);
		    	break;
		    case ':':       /* missing operand */
		      	cerr << argv[0] << ": Option -" << optopt << " requires an operand." << endl;
		      	usage(argv[0], EXIT_FAILURE);
		     	break;

		    case '?':       /* unknown option */
		      	//cerr << argv[0] << ": Unrecognized option -" << optopt << endl;
		      	usage(argv[0], EXIT_FAILURE);
   		}
    }
}

void run_prog_from_parameters(int argc, char *argv[])
{

	// Variables to read the arguments
	char *in_file;
	int max_stack = -1;
	int cost_cv, cost_ec = -1 ;

	// Parse main arguments
	parse_args(argc, argv, in_file, max_stack, cost_cv, cost_ec);
	if ( (max_stack <= 0) || (cost_cv <= 0) || (cost_ec <= 0) || (in_file[0] == '\0'))
	{
		usage(argv[0], EXIT_SUCCESS);
	}
	    
	// Create the simulator and the network from input file 
	Simulation *simulation = new Simulation(in_file, max_stack, cost_cv, cost_ec);

	// Start the simulation 
	simulation->start();

	// Stop the simulation
	simulation->stop();

	// Print the convergence time
	cout << "The convergence time is " << setprecision(5) << fixed << simulation->get_convergence_time() << " seconds !" << endl; 

	// Delete the simulator 
	delete simulation;			
}

int main(int argc, char *argv[])
{
	switch (argc)
	{
		case 1:
			usage(argv[0], EXIT_FAILURE);
			break;
		default:
			run_prog_from_parameters(argc, argv);
			break;      
	}
	return EXIT_SUCCESS;
}