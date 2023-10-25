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

#include "AdaptationFunction.hpp"


AdaptationFunction::AdaptationFunction(int _type, char _from, char _to)
{
	type = _type; 
	from = _from;
	to   = _to;
}

int AdaptationFunction::get_type()
{
	return type;
}

char AdaptationFunction::get_from()
{
	return from;
}

char AdaptationFunction::get_to()
{
	return to;
}

void AdaptationFunction::display()
{
	switch (type) 
    {
      case CV: 
      	cout << "Cv " << from << " to " << to << endl;
      	break;
      case EC:  
        cout << "Ec " << from << " in " << to << endl;
      	break;
      case DC:  
        cout << "Dc " << to << " from " << from << endl;
      	break;
      default:
      	cerr << "Invalid type of Adaptation function " << endl; 
      	break;
    }
}

AdaptationFunction *AdaptationFunction::reverse()
{
	switch(type) 
	{
	    case CV :
	    	return new AdaptationFunction(CV, to, from);
	        break;
	    case EC :
	    	return new AdaptationFunction(DC, to, from);
	    	break;
	    case DC :
	    	return new AdaptationFunction(EC, to, from);
	        break;
	    default :
	        cerr << "Invalid type adaptation function" << endl;
	        return nullptr;
	}
} 

bool AdaptationFunction::applicable(string protocol_stack)
{
	if (!protocol_stack.empty())
	{
		char top_protocol, top_protocol_next;
	   	switch(type) 
	   	{
		    case CV :
		    	top_protocol = protocol_stack.back();
		        if(top_protocol == from) 
		    	    return true;
		        else
		        	return false;
		        break;
		    case EC :
		    	top_protocol = protocol_stack.back();
		    	if ( top_protocol == from )
		    		return true;
		    	else 
		    		return false;
		    	break;
		    case DC :
		    	top_protocol = protocol_stack.back();
		    	protocol_stack.pop_back();
		    	if(!protocol_stack.empty())
		    	{
		    		top_protocol_next = protocol_stack.back();
					if ( ( top_protocol == from ) && ( top_protocol_next == to ) )
						return true;
		    	}
				return false;	    	
		        break;
		    default :
		        cerr << "Invalid type of adaptation function ! " << endl;
		        return false;
	    }
	}
	return false;
}

bool AdaptationFunction::apply_to_stack(string& protocol_stack)
{
	bool result = applicable(protocol_stack);
	if(result)
	{
		switch(type) 
	   	{
		    case CV :
		        protocol_stack.pop_back();
		    	protocol_stack.push_back(to);
		        break;
		    case EC :
				protocol_stack.push_back(to);
		    	break;
		    case DC :
				protocol_stack.pop_back();    	
		        break;
		    default :
		        cerr << "Invalid type of adaptation function ! " << endl;
	    }	
	}
	return result;
}