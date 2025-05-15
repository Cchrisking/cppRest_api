CXX = g++
CXXFLAGS = -std=c++17 -O3 -pthread -I/usr/include/nlohmann
LDFLAGS = -lcurl -lpthread

# Targets
all: book_api_load_test book_api_rps_test
#user_api_load_test user_api_rps_test

book_api_load_test: book_api_load_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

book_api_rps_test: book_api_rps_test.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f book_api_load_test book_api_rps_test

run_load: book_api_load_test
	./book_api_load_test

run_rps: book_api_rps_test
	./book_api_rps_test --rps 1000 --duration 10

.PHONY: all clean run_load run_rps