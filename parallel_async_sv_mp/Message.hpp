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

#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <iostream>
#include <stack>

using namespace std;

class Message 
{
	private : 
	    int dest;
	    int cost;
	    string protocol_stack; 
	public : 
		Message(int _dest, string _protocol_stack, int _cost);
		int get_dest();
		int get_cost();
		string get_stack();
		void display();
};


#endif