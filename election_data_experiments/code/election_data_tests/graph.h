// Graph class

#include <bits/stdc++.h>

using namespace std;

class Graph {
    public:
        int V;
        vector < vector <int > > adj;
        vector < vector < int > > SCCs;
        void fillOrder(int s, vector<bool> &visitedV, stack<int> &Stack);
        void KosarajuDFS(int s, int scc_number, vector <bool> &visitedV);

        void getSCCsize(){
            cout << SCCs.size() << endl;
        }

        //Graph Construction
        Graph(int V);
        void addEdge(int s, int d);
        void CalcSCC();
        Graph transpose();
};