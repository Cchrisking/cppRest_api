CXX = g++
CXXFLAGS = -std=c++17 -O3 -pthread
LIBS = -lcurl -lnlohmann_json

all: book_api_load_test book_api_rps_test

book_api_load_test: test_book_api_load.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

book_api_rps_test: test_book_api_rps.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

clean:
	rm -f book_api_load_test book_api_rps_test

.PHONY: all clean