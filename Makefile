CXX = g++
CXXFLAGS = -std=c++17 -Wall
TARGET = loadbalancer
SRCS = main.cpp webserver.cpp request_generator.cpp load_balancer.cpp high_level_load_balancer.cpp
OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
