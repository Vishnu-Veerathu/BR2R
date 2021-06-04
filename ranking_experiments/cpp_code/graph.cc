// Graph class

#include <bits/stdc++.h>
#include "graph.h"

using namespace std;

Graph::Graph(int V) : adj(V, vector <int>())
{
    this->V = V;
}

void Graph::KosarajuDFS(int s, int scc_number, vector <bool> &visitedV){
    visitedV[s] = true;
    SCCs[scc_number].push_back(s);

    for(vector<int>::iterator i = adj[s].begin(); i!=adj[s].end(); ++i){
        if(!visitedV[*i]){
            KosarajuDFS(*i,scc_number,visitedV);
        }
    }
}

// Transpose
Graph Graph::transpose() {
  Graph g(V);
  for (int s = 0; s < V; s++) {
    vector<int>::iterator i;
    for (i = adj[s].begin(); i != adj[s].end(); ++i) {
      g.adj[*i].push_back(s);
    }
  }
  return g;
}

// Add edge into the graph
void Graph::addEdge(int s, int d) {
  adj[s].push_back(d);
}

void Graph::fillOrder(int s, vector<bool> &visitedV, stack<int> &Stack) {
  visitedV[s] = true;

  vector<int>::iterator i;
  for (i = adj[s].begin(); i != adj[s].end(); ++i)
    if (!visitedV[*i])
      fillOrder(*i, visitedV, Stack);

  Stack.push(s);
}

// Print strongly connected component
void Graph::CalcSCC() {
  stack<int> Stack;

  vector<bool>visitedV(V,false);

  for (int i = 0; i < V; i++)
    if (visitedV[i] == false)
      fillOrder(i, visitedV, Stack);

  Graph gr = transpose();

  for (int i = 0; i < V; i++)
    visitedV[i] = false;

  int scc_number = 0;
  while (Stack.empty() == false) {
    int s = Stack.top();
    Stack.pop();

    if (visitedV[s] == false) {
      while(scc_number >= gr.SCCs.size()){
        gr.SCCs.push_back(vector<int>());
      }
      gr.KosarajuDFS(s, scc_number, visitedV);
      scc_number++;
    }
  }
  SCCs = gr.SCCs;
}