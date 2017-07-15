cc = g++
flag = -std=c++11 -lpthread
OBJ = dfs_2.o

dfs_2:$(OBJ)
	$(cc) -o $@ $^ $(flag)

$(OBJ):dfs_2.cpp
	$(cc) -c $^ $(flag)

clean:
	rm -rf *.o dfs_2
