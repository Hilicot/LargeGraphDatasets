#include "Graph.h"
#include <queue>
#include <random>
#include <iostream>

Graph::Graph(int V, int E) {
    this->V = V;
    this->E = E;
    adj.resize(V);
}

void Graph::addEdge(int u, int v) {
    // order vertices
    if (v < u) {
        int temp = u;
        u = v;
        v = temp;
    }

    // check if edge already exists
    if (std::binary_search(adj[u].begin(), adj[u].end(), v))
        return;

    // insert edge
    auto it = std::lower_bound(adj[u].begin(), adj[u].end(), v);
    adj[u].insert(it, v);
}

int Graph::getNumVertices() const {
    return V;
}

int Graph::getNumEdges() const {
    return E;
}

vector<int> Graph::getAdjacentVertices(int v) {
    return adj[v];
}

vector<int> Graph::sampleConnectedVertices(int N) const {
    //Select a random vertex v in the graph.
    int v = rand() % V;

    // Perform a Breadth-First Search (BFS) or Depth-First Search (DFS) starting from v, and mark all the visited vertices.
    vector<bool> visited(V, false);
    queue<int> q;
    q.push(v);
    visited[v] = true;
    int numVisited = 1;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (int w : adj[u]) {
            if (!visited[w]) {
                visited[w] = true;
                q.push(w);
                numVisited++;
            }
        }
    }

    // If the number of marked vertices is less than N, go to step 1 and select another random vertex that has not been visited yet.
    while (numVisited < N) {
        v = rand() % V;
        if (!visited[v]) {
            // Perform BFS or DFS from v
            queue<int> q;
            q.push(v);
            visited[v] = true;
            numVisited++;
            while (!q.empty() && numVisited < N) {
                int u = q.front();
                q.pop();
                for (int w : adj[u]) {
                    if (!visited[w]) {
                        visited[w] = true;
                        q.push(w);
                        numVisited++;
                    }
                }
            }
        }
    }

    // If the number of marked vertices is greater than or equal to N, select N vertices randomly from the marked vertices and return them as the result.
    vector<int> sampledVertices;
    for (int i = 0; i < V; i++) {
        if (visited[i]) {
            sampledVertices.push_back(i);
        }
    }
    sort(sampledVertices.begin(), sampledVertices.end());
    sampledVertices.resize(N);
    return sampledVertices;
}

Graph Graph::sample(int N, int E) const {
    if (N > this->V)
        N = this->V;
    if (E > this->E) {
        E = this->E;
    }
    cout << "Sampling " << N << " vertices and " << E << " edges" << endl;

    // sample vertices
    vector<int> vertices = this->sampleConnectedVertices(N);

    // keep only valid edges, and relabel vertices from 0 to N-1
    int numVertices = vertices.size();
    Graph subgraph(numVertices, 0);
    int numEdges = 0;
    Relabeller relabeller(vertices);
    vector<pair<int, int>> edges;
    for (int i = 0; i < N; i++) {
        int u = vertices[i];
        for (int v : adj[u]) {
            if (!binary_search(vertices.begin(), vertices.end(), v)) {  // if v in vertices
                continue; // edge does not connect two vertices in the subgraph
            }
            int u_label = relabeller.getLabel(u);
            int v_label = relabeller.getLabel(v);
            if(E < 0) { // if we don't sample edges, add edge now, else add edge later after sampling
                subgraph.addEdge(u_label, v_label);
                numEdges++;
            }else
                edges.push_back(make_pair(u_label, v_label));
        }
    }

    // if E != -1, sample edges
    if (E >= 0) {
        shuffle(edges.begin(), edges.end(), std::mt19937(std::random_device()()));
        if(E < edges.size())
            edges.resize(E);
        else
            E = edges.size();

        // add sampled edges
        for (auto edge : edges) {
            int u = edge.first;
            int v = edge.second;
            subgraph.addEdge(u, v);
        }
        subgraph.E = E;
    }else{
        subgraph.E = numEdges;
    }
    return subgraph;
}

