all: radio.cpp
	clang++ -Wall radio.cpp
	./cleaner < radio.cpp > radio_clean.cpp
debug: radio.cpp
	clang++ -Wall radio.cpp -DDEBUG