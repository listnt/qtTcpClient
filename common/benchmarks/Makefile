CXX = g++ # replace with your C++ compiler
CXXFLAGS = -std=c++20 -O3

all: benchmark_cout benchmark_logger benchmark_os

benchmark_cout:
	$(CXX) $(CXXFLAGS) -o cout_benchmark.out cout_benchmark.cpp
	sudo perf stat -r 100 ./cout_benchmark.out>>/dev/null
benchmark_logger:
	$(CXX) $(CXXFLAGS) -o logger_benchmark.out logger_benchmark.cpp
	sudo perf stat -r 100 ./logger_benchmark.out>>/dev/null
benchmark_os:
	$(CXX) $(CXXFLAGS) -Wno-unused-result -o os_benchmark.out os_benchmark.cpp
	sudo perf stat -r 100 ./os_benchmark.out>>/dev/null