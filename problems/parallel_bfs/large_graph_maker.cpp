#include<vector>
#include<random>
#include<chrono>
using namespace std;

const int NUM_NODES=10000;
const int EDGES_PER_NODE=10;

vector<vector<int>> generate_graph(int num_nodes, int edges_per_node)
{
	vector<vector<int>> adj(num_nodes);
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(0, num_nodes -1);

	for(int u=0;u<num_nodes;u++)
	{
		for(int i=0;i<edges_per_node;i++)
		{
			int v=dis(gen);
			if(v!=u)
			{
				adj[u].push_back(v);
				adj[v].push_back(u);
			}
		}
	}
	return adj;
}
