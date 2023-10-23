CCXX = g++        
CXXFLAGS = -O3 -Wall -Wextra -std=c++17 
LDFLAGS = -ldl -L/usr/local/lib 


SRC_PROG = $(wildcard *.cpp)
OBJ_PROG = $(SRC_PROG:.cpp=.o)


prog: $(OBJ_PROG)
	$(CXX) $^ -o $@ $(LDFLAGS) 
	@echo "****************Make Program Successful********************"   

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ -c $<


clean :
	$(RM) -f prog $(OBJ_PROG) 
	@echo "************Clean Program Successful**************"