#include<iostream>
#include<thread>
#include<mutex>
#include<queue>
#include<condition_variable>
#include<functional>
#include<string>
#include<atomic>
#include<fstream>
#include<sstream>
#include<iomanip>
#include<cstdlib>
using namespace std;


void print_adj_out(vector<vector<pair<int,double>>> &adj_out)
{
	int n=adj_out.size();
	for(int i=0;i<n;i++)
	{
		cout << "Node " << n << ": ";
		for(auto it: adj_out[i])
		{
			cout << "(" << it.first << "," << it.second << ") ";
		}
		cout << endl;
	}
}


vector<vector<pair<int,double>>> read_graph(string filename)
{
	ifstream in(filename);
	if(!in)
	{
		throw runtime_error("Error opening graph.txt");
	}

	int n;
	in >> n;
	vector<vector<pair<int,double>>> adj_out(n);
	string line;
	getline(in,line);							//getting rid of endline.

	for(int i=0;i<n;i++)
	{
		getline(in,line);
		istringstream iss(line);
		int m;
		iss >> m;
		for(int j=0;j<m;j++)
		{
			int dst;
			double edge_prob;
			iss >> dst >> edge_prob;
			adj_out[i].emplace_back(dst,edge_prob);
		}
	}
	return adj_out;
}

class Actor
{
	private:
		queue<double> inbox;
		mutex mtx;
		condition_variable cv;
		int id;
		double state;
		double prev_state;
		int in_deg;
		vector<pair<double,function<void(double)>>> send_fns;

	public:
		Actor(int id_arg,double state_arg) : id(id_arg), state(state_arg), in_deg(0), prev_state(state_arg)
		{}

		void inbox_push(double v)
		{
			{
				lock_guard<mutex> lk(mtx);
				inbox.push(v);
			}
			cv.notify_one();
		}

		void add_send_fn(double edge_prob, function<void(double)> push_fun)
		{
			(this->send_fns).push_back({edge_prob,push_fun});
		}

		void send_phase()
		{
			double my_val=state;
			for(auto &f: send_fns)
			{
				//cout << "Actor " << this->id << " sending out value: " << (my_val * f.first) << endl;
				f.second(my_val * f.first);
			}
		}

		void recv_phase()
		{
			double sum=0.0;
			for(int i=0;i<in_deg;i++)
			{
				unique_lock<mutex> lk(mtx);
				cv.wait(lk, [&]{return !inbox.empty();});
				sum+=inbox.front();
				//cout << "Actor " << this->id << " received value idx " << i << " : " << (inbox.front()) << endl;
				inbox.pop();
			}
			state=sum;
		}

		void print_info()
		{
			cout << "Actor " << this->id << " ; current state: " << fixed << setprecision(4) << this->state << " and prev_state: " << fixed << setprecision(4) << this->prev_state << endl;
		}

		void inc_in_deg()
		{
			this->in_deg += 1;
		}

		void set_prev_state()
		{
			this->prev_state = this->state;
		}

		bool has_converged(double threshold)
		{
			return abs(this->state - this->prev_state)<=threshold;
		}
};


void pagerank_loop(vector<Actor*> actors, double threshold)
{
	int rec_cnt=52;
	int n=actors.size();
	while(rec_cnt--)
	{
		//counting the actors that haven't converged yet.
		int cnt_of_non_converged_actors=0;
		for(int i=0;i<n;i++)
		{
			if(!actors[i]->has_converged(threshold))
			{
				cnt_of_non_converged_actors++;
			}
		}

		if(!(rec_cnt==51) && (cnt_of_non_converged_actors==0))
		{
			cout << "Actors have now stabilised\n";
			break;
		}

		//sending the messages.
		for(int i=0;i<n;i++)
		{
			actors[i]->set_prev_state();
			actors[i]->send_phase();
		}

		//receiving the messages and updating states.
		for(int i=0;i<n;i++)
		{
			actors[i]->recv_phase();
		}

		//Printing the updated states.
		/*
		cout << "Current loop: " << (52-rec_cnt) << endl;
		for(int i=0;i<n;i++)
		{
			actors[i]->print_info();
		}
		*/
	}
}

int main()
{
	srand(time(0));
	auto adj_out = read_graph("graph.txt");
	//print_adj_out(adj_out);
	
	double threshold=1.5;

	int n=adj_out.size();
	vector<Actor*> actors;
	for(int i=0;i<n;i++)
	{
		Actor *a = new Actor(i,rand()%10 * 1.0);
		actors.push_back(a);
	}

	//for(auto neighbours: adj_out)
	for(int i=0;i<n;i++)
	{
		auto neighbours=adj_out[i];
		for(auto edge : neighbours)
		{
			auto dst = edge.first;
			auto edge_prob=edge.second;
			actors[dst]->inc_in_deg();

			auto pusher = bind(&Actor::inbox_push, actors[dst],placeholders::_1);
			actors[i]->add_send_fn(edge_prob,pusher);
		}
	}

	pagerank_loop(actors,threshold);

	return 0;
}
