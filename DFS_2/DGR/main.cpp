#include "graph.h"
#include <iostream>
#include <cstring>

using namespace std;

int main(int argc, char**argv)
{
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

	//check result
	for(int i=0;i< (gr->vert_count > 8? 8:gr->vert_count);i++)
	{
		cout<<Beg[i]<<" ";
	}

	cout<<endl;
	cout<<endl;

	for(int i=0;i<gr->edge_count;i++)
	{
		cout<<Adj[i]<<" ";
	}
	return 0;
}
