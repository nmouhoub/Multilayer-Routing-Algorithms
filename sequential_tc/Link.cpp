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

#include "Link.hpp"


Link::Link(int _src, int _dest)
{
	src = _src;
	dest = _dest;
}

int Link::get_src()
{
	return src;
}

int Link::get_dest()
{
	return dest;
}

MapCost Link::get_map_cost()
{
	return map_cost;
}

void Link::set_map_cost(MapCost _map_cost)
{
	map_cost = _map_cost;
}

int Link::get_cost(AdaptationFunction *adapt_func)
{
	return map_cost.find(adapt_func)->second;
}