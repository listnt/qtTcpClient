CXX = g++ # replace with your C++ compiler
CXXFLAGS = -std=c++20 -O0

all: detect_leaks_valgrind detect_leaks_sanitize 

detect_leaks_valgrind:
	$(CXX) $(CXXFLAGS) -o exception_occurred.out exception_ocurred.cpp
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./exception_occurred.out
detect_leaks_sanitize:
	$(CXX) $(CXXFLAGS) -fsanitize=address -o exception_occurred.out exception_ocurred.cpp
	./exception_occurred.out


true_negative: true_negative_valgrind true_negative_sanitize 
true_negative_valgrind:
	$(CXX) $(CXXFLAGS) -o true_negative.out true_negative.cpp
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./true_negative.out
true_negative_sanitize:
	$(CXX) $(CXXFLAGS) -fsanitize=address -o true_negative.out true_negative.cpp
	./true_negative.out