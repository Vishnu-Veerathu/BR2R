#include <bits/stdc++.h>
#include "graph.h"

using namespace std;

bool debug = false;

vector<int> stovi(string str)
{
	std::replace(str.begin(), str.end(), ',', ' ');
	vector<int> array;
	stringstream ss(str);
	int temp;
	while (ss >> temp)
		array.push_back(temp);
	return array;
}

int main()
{
	int n;
	cin >> n;
	for (int i = 0; i < n + 2; i++)
	{
		string s;
		getline(cin, s);
	}

	Graph g(n);
	vector<vector<int>> adjm(n, vector<int>(n, 0));
	vector<pair<int, int>> edges;
	int min_index = 1e9;
	for (int i = 0; i < n * (n - 1) / 2; i++)
	{
		string str;
		getline(cin, str);
		auto v = stovi(str);
		edges.push_back({v[1], v[2]});
		min_index = min(min_index, v[1]);
		min_index = min(min_index, v[2]);
	}

	for (auto &edge : edges)
	{
		int s = edge.first - min_index;
		int t = edge.second - min_index;
		g.addEdge(s, t);
		adjm[s][t] = 1;
		adjm[t][s] = -1;
	}

	if (debug)
	{
		cout << "n = " << n << endl;
	}

	g.CalcSCC();
	bool is_br2 = true;
	int violations = 0;
	if (debug)
	{
		for (int i = 0; i < g.SCCs.size(); i++)
		{
			cout << "SCC size = " << g.SCCs[i].size() << endl;
			cout << "SCC members = ";
			for (auto e : g.SCCs[i])
			{
				cout << e + 1 << " ";
			}
			cout << endl;
		}
	}
	for (int i = 0; i < g.SCCs.size(); i++)
	{
		for (int a = 0; a < g.SCCs[i].size(); a++)
		{
			for (int b = 0; b < g.SCCs[i].size(); b++)
			{
				if (b == a)
					continue;
				for (int c = 0; c < g.SCCs[i].size(); c++)
				{
					if (c == b || c == a)
						continue;
					if (!(adjm[a][b] > 0 && adjm[b][c] > 0 && adjm[c][a] > 0))
						continue;
					for (int d = 0; d < g.SCCs[i].size(); d++)
					{
						if (d == c || d == b || d == a)
							continue;
						if (adjm[d][a] == adjm[d][b] && adjm[d][a] == adjm[d][c])
						{
							is_br2 = false;
							violations++;
						}
					}
				}
			}
		}
	}

	// for (int i = 0; i < n; i++)
	// {
	// 	for (int j = 0; j < n; j++)
	// 	{
	// 		cout << adjm[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }

	if (n <= 20)
	{
		// cout << "No No" << endl;
		return 0;
	}

	if (n == g.SCCs.size())
	{
		cout << "Yes ";
	}
	else
	{
		cout << "No ";
	}

	if (is_br2)
	{
		cout << "Yes" << endl;
	}
	else
	{
		cout << "No" << endl;
	}
}