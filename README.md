# Multilayer Routing Algorithms

This project is a set of multilayer routing algorithms written in C++ and published in the following articles:

* A stack-vector routing protocol for automatic tunneling [Mohamed Lamine Lamali et al., 2019] (https://ieeexplore.ieee.org/abstract/document/8737536)
* A Highly Parallelizable Algorithm for Routing With Automatic Tunneling [Noureddine Mouhoub et al.] (https://ieeexplore.ieee.org/document/9829784)

## Requirements

You will need to have or install all the following software packages:

* C++17 capable compiler
* make 4.0 or newer
* (optional) [Doxygen](https://www.doxygen.nl/) for docs
* (optional) [Graphviz](https://graphviz.org/) for more graphs in docs
* (optional) [PlantUML](https://plantuml.com/) for more UML diagrams (PlantUML needs Java)

## Building the program

For building the program, just type the following command in the directory of the chosen algorithm:

```
make prog
```

## Usage 

Run the program with the input file that contains the multilayer network topology, the maximum protocol stack height, the cost of conversion and the cost of encapsulation.

```
./prog -i <topology_file> -s <maximum_protocol_stack_height> -c <conversion_cost> -e <encapsulation_cost> 

```

The meaning of each program option:

* i: input_file topology (non empty file)
* s: maximum protocol stack height (non negative)
* c: conversion cost (non negative)
* e: encapsulation cost (non negative)
* h: help

```
```
## Examples

Sample command line to run the program:

```
./prog -i ../multilayer_random_topologies/n_100_m_3_p_0.10_topo_0.txt -s 3 -c 1 -e 1

```

## Documentation
<!---
Use the following command to run the automated documentation for this project:

```
make doc
./doc
```

-->

Source code documentation is available in `random-generator/doc/html/index.html`.

Documentation is available on the [Wiki](https://gitub.u-bordeaux.fr/hera/multilayer-routing-algorithms).

## Licence

GNU LGPL version 3.

## Version

1.0

## Author

* **Noureddine Mouhoub** - (*GitHub:* [NoureddineMouhoub](https://github.com/nmouhoub))
