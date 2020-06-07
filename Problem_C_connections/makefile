all: conn.cpp
	g++ -std=c++11 -Wall -O2 -static -o a.out conn.cpp
	#./cleaner < conn.cpp > conn_clean.cpp
debug: conn.cpp
	clang++ -Wall conn.cpp -DDEBUG