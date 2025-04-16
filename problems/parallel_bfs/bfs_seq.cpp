#include<iostream>
#include<vector>
#include<queue>

using namespace std;

void seq_bfs(int start, const vector<vector<int>> &adj, vector<int> &level)
{
	queue<int> q;
	q.push(start);
	level[start]=0;

	while(!q.empty())
	{
		int node=q.front();
		q.pop();
		cout << node << " ";
		for(int neighbor: adj[node])
		{
			if(level[neighbor]==-1)
			{
				level[neighbor]=level[node]+1;
				q.push(neighbor);
			}
		}
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

	seq_bfs(0,adj,level);

	cout << "Node Levels from source 0 (Sequential BFS): " << endl;
	for(int i=0;i<V;i++)
	{
		cout << "Node " << i << ": Level " << level[i] << endl;
	}
	return 0;
}
