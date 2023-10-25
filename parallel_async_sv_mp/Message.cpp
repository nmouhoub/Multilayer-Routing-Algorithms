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

#include "Message.hpp"


Message::Message(int _dest, string _protocol_stack, int _cost)
{
	dest = _dest; 
	cost = _cost;
	protocol_stack = _protocol_stack;
}

string Message::get_stack()
{
	return protocol_stack;
}

int Message::get_dest()
{
	return dest;
}

int Message::get_cost()
{
	return cost;
}

void Message::display()
{
	cout << "===============" << endl;
	cout << "dest: " <<  dest << endl; 
	cout << "cost: " << cost << endl; 
	cout << "stack: " << protocol_stack << endl; 
	cout << "===============" << endl;
}