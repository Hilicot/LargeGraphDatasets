#ifndef LARGEDATASETS_GRAPH_H
#define LARGEDATASETS_GRAPH_H
#include <vector>
#include <algorithm>
#include <unordered_map>

using namespace std;

class Relabeller {
public:
    Relabeller(vector<int> vertices) {
        for (int i = 0; i < vertices.size(); i++) {
            labelMap[vertices[i]] = i;
        }
    }

    int getLabel(int oldLabel) {
        return labelMap[oldLabel];
    }

private:
    std::unordered_map<int, int> labelMap;
};

class Graph {
public:
    Graph(int V, int E);

    int addNode();

    void addEdge(int u, int v);

    int getNumVertices() const;

    int getNumEdges() const;

    int get(const int u, const int v) const;

    vector<int> getAdjacentVertices(int v);

    Graph sample(int N, int E) const;


private:
    int V;
    int E;
    vector<vector<int>> adj;

    vector<int> sampleConnectedVertices(int N) const;
};

#endif //LARGEDATASETS_GRAPH_H
