#include<iostream>
#include<vector>
#include<queue>
#include<omp.h>
#include<set>

using namespace std;

void parallel_bfs(int start, const vector<vector<int>> &adj, vector<int> &level)
{
	int V=adj.size();
	vector<int> current_frontier;
	current_frontier.push_back(start);
	level[start]=0;

	int curr_level=0;

	while(!current_frontier.empty())
	{
		vector<int> next_frontier;

		//The below line creates the threads to run the program in parallel.
#pragma omp parallel
		{
			vector<int> local_frontier;

#pragma omp for nowait
			for(int i=0;i<current_frontier.size();i++)
			{
				int node=current_frontier[i];
				for(int neighbor: adj[node])
				{
					if(level[neighbor]==-1)
					{
#pragma omp critical
						{
							if(level[neighbor]==-1)
							{
								level[neighbor]=curr_level+1;
								local_frontier.push_back(neighbor);
							}
						}
					}
				}
			}

#pragma omp critical
			next_frontier.insert(next_frontier.end(), local_frontier.begin(), local_frontier.end());
		}

		current_frontier=next_frontier;
		curr_level++;
	}
}


int main()
{
	int V=6;	//Number of vertices in the graph.
	vector<vector<int>> adj(V);

	adj[0]={1,2};
	adj[1]={0,3,4};
	adj[2]={0,4};
	adj[3]={1,5};
	adj[4]={1,2,5};
	adj[5]={3,4};

	vector<int> level(V, -1);

	parallel_bfs(0,adj,level);

	cout << "Node Levels from source 0 (Parallel BFS): " << endl;
	for(int i=0;i<V;i++)
	{
		cout << "Node " << i << ": Level " << level[i] << endl;
	}
	return 0;
}
