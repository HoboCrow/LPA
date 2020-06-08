all: conn.cpp
	g++ -std=c++11 -Wall -o a.out -g conn.cpp
	# g++ -std=c++11 -Wall -O2 -static -o a.out -g conn.cpp
	#./cleaner < conn.cpp > conn_clean.cpp
debug: conn.cpp
	clang++ -Wall conn.cpp -DDEBUG