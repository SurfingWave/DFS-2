#include "DGR/graph.h"

#include <stdio.h>
#include <iostream>
#include <thread>
#include <stdlib.h>
#include <deque>
#include <condition_variable>
#include <pthread.h>

using namespace std;

bool start = false;
bool ready = false;

condition_variable syn_start;
condition_variable frontier_ready;
mutex mtx;

//#define v_num 8
//#define e_num 10
#define thresh_hold 2
#define thread_num 2
                 //0     1   2     3  4  5  6
//int Beg[v_num] = { 0,    2,  3,    5, 6, 7, 8 };
//int Adj[e_num] = { 2, 4, 5,  3, 1, 5, 6, 6, 1 };
//bool visited[v_num] = { 0, 0, 0, 0, 0, 0, 0, 0 };

deque<int>worker_local_stack;

void DFS_master(deque<int>&root_local_stack, long int edge_num,
	long int *Beg, long int *Adj, bool* visited)
{
	while (!start)
		sleep(1);

	while (!root_local_stack.empty())
	{
		int cur = root_local_stack.back();
		root_local_stack.pop_back();

		if (!visited[cur])
		{
			visited[cur] = true;

			cout << "root:  " << cur << endl;

			for (int i = Beg[cur]; i < Beg[cur + 1]&& i < edge_num; i++)
				if (!visited[Adj[i]])
					root_local_stack.push_back(Adj[i]);

		}
		//Equal-size partition of the large frontier
		int size = root_local_stack.size();

		if (size >= thresh_hold)
		{
			int temp = thresh_hold / 2;

			while (temp > 0)
			{
				worker_local_stack.push_back(root_local_stack.front());
				root_local_stack.pop_front();
				temp--;
			}
			//need to verify whether the above has been successfully down
			// set the condition variable then notify.
			ready = true;
			frontier_ready.notify_all();
		}
	}
}

void DFS_worker(long int edge_num, long int *Beg, long int *Adj, bool* visited)
{
	while (!ready)
		sleep(1);

	//cout << "thread2: hello"<< worker_local_stack.size() << endl;

	while (!worker_local_stack.empty())
	{
		int cur = worker_local_stack.back();
		worker_local_stack.pop_back();

		if (!visited[cur])
		{
			visited[cur] = true;

			cout << "worker:  " << cur << endl;

			for (int i = Beg[cur]; i < Beg[cur + 1] && i < edge_num; i++)
				if (!visited[Adj[i]])
					worker_local_stack.push_back(Adj[i]);
		}

		//Equal-size partition of the large frontier
		/*int size = worker_local_stack.size();

		if (size > thresh_hold)
		{
		int temp = thresh_hold / 2;
		int splitted_frontier[thresh_hold / 2];
		while (temp > 0)
		{
			splitted_frontier[thresh_hold / 2 - temp] = worker_local_stack.front();
			worker_local_stack.pop_front();
			temp--;
		}
	  }*/
	}
	return;
}

int main(int argc, char **argv)
{
	thread t[thread_num];

	deque<int>g;
	g.push_back(0);

	if (argc < 3)
	{
		cout << "wrong number of arguments" << endl;
		cout << "./exe graph_begin_file graph_csr_file" << endl;
		exit(-1);
	}

	graph<long int,long int,long int,long int> *gr = new graph<long int,long int,long int,long int>(argv[1], argv[2]);

	long int *Beg = new long int[gr->vert_count];
	long int *Adj = new long int[gr->edge_count];

	memcpy(Beg, gr->beg_pos, sizeof(long int) * gr->vert_count);
	memcpy(Adj, gr->csr, sizeof(long int) * gr->edge_count);

    bool *visited = new bool[gr->vert_count];
    memset(visited, 0, gr->vert_count*sizeof(bool));


	t[1] = thread(&DFS_worker, gr->edge_count, Beg, Adj, visited);    // activate the worker first

	start = true;
	syn_start.notify_all();
	t[0] = thread(&DFS_master, ref(g), gr->edge_count, Beg, Adj, visited);  //start up master thread
	

	for (int thr = 0; thr < thread_num; thr++)
		t[thr].join();          // synchronization of all threads

    return 0;
}