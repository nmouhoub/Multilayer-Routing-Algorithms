#ifndef ADAPTATION_FUNCTION_HPP
#define ADAPTATION_FUNCTION_HPP

#include <iostream> 
#include <utility>
#include <stack> 

using namespace std;

# define CV  0
# define EC  1
# define DC  2

class AdaptationFunction
{
    private:
	    int type; 
		char from; 
		char to; 

    public:		
    	AdaptationFunction(int _type, char _from, char _to);
		int get_type();
		char get_from();
    	char get_to(); 
    	void display();
    	AdaptationFunction *reverse();
    	bool applicable(string protocol_stack);
    	bool apply_to_stack(string& protocol_stack);
};


#endif