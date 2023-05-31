#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <chrono>
#include <sys/stat.h>
#include <sstream>
#include "Graph.h"

#define VERBOSE 1
#define MIN_NODES 150  // don't convert graphs with less than MIN_NODES nodes

#define SAMPLING false
#define N 100000
#define CONNECTIVITY 5.0 // define average number of edges per vertex (if the graph is less connected than this, don't sample edges)
#define OFFSET_STDEV 5.0 // define percentual variation for N and E across the 2 graphs

#define GENERATE_SYNTHETIC_PAIR true
#define ADDED_NODES_PROB 5 // %     (x/100)
#define ADDED_EDGES_PROB 10 // %*%   (x/10000)

using namespace std;

// NOTE: graphs are loaded discarding all non necessary info. The output graph is undirected, unweighted and unlabeled

// Function to read a graph from a file
Graph readGraphFromFile(string filename) {
    ifstream infile(filename);

    if (!infile) {
        cerr << "Error: cannot open file " << filename << endl;
        exit(1);
    }

    // read header
    int V, E;
    string line;
    getline(infile, line);
    if (line[0] == '#') {
        for (int i = 0; i < 3; i++)
            if (!getline(infile, line) || line[0] != '#') {
                cerr << "Error: invalid file format" << endl;
                exit(1);
            }
        stringstream ss (line);
        string tmp_str;
        getline(ss, tmp_str, ':');
        getline(ss, tmp_str, ' ');
        V = stoi(tmp_str);
        getline(ss, tmp_str, ':');
        getline(ss, tmp_str, ' ');
        E = stoi(tmp_str);
        getline(infile, line);
    } else {
        V = stoi(line.substr(0, line.find(' ')));
        E = stoi(line.substr(line.find(' ') + 1));
    }

    // read edges
    Graph G(V, E);
    unordered_map<unsigned int, int> vertex_map; // map old vertex labels to new labels
    int vertex_count = 0;
    unsigned int u, v;
    for (long int i = 0; i < E; i++) {
        if (VERBOSE && i > 0 && i % 1000000 == 0)
            std::cout << "reading edges: " << i * 100 / E << "%     " << i << "/" << E << std::endl;

        // Read the next edge
        infile >> u >> v;

        // Discard any extra values on this line
        char c;
        while (infile.get(c) && c != '\n');

        // Check if u is a new vertex
        if (vertex_map.find(u) == vertex_map.end()) {
            vertex_map[u] = vertex_count++;
        }
        // Check if v is a new vertex
        if (vertex_map.find(v) == vertex_map.end()) {
            vertex_map[v] = vertex_count++;
        }

        int new_u = vertex_map[u];
        int new_v = vertex_map[v];
        G.addEdge(new_u, new_v);
    }

    infile.close();

    return G;
}

// Function to save a graph to a file
void saveGraphToFile(Graph G, string filename) {
    ofstream outfile(filename);

    if (!outfile) {
        cerr << "Error: cannot open file " << filename << endl;
        exit(1);
    }

    outfile << G.getNumVertices() << " " << G.getNumEdges() << endl;

    for (int v = 0; v < G.getNumVertices(); v++) {
        if (VERBOSE && v > 0 && v % 50000 == 0)
            std::cout << "writing node " << v * 100 / G.getNumVertices() << "%     " << v << "/" << G.getNumVertices()
                      << std::endl;
        vector<int> adj = G.getAdjacentVertices(v);
        for (int i = 0; i < adj.size(); i++) {
            outfile << v << " " << adj[i] << endl;
        }
    }

    outfile.close();
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " input_file output_folder [counter]" << endl;
        exit(1);
    }

    string input_file = argv[1];
    string output_folder = argv[2];

    // read graph
    std::cout << "Reading " << input_file << std::endl;
    Graph G = readGraphFromFile(input_file);

    // Extract the filename from the input file path
    size_t last_slash_idx = input_file.find_last_of("/\\");
    string filename = input_file.substr(last_slash_idx + 1);

    // Remove the file extension from the filename
    size_t last_dot_idx = filename.rfind('.');
    string basename = (last_dot_idx == string::npos) ? filename : filename.substr(0, last_dot_idx);
    if(basename.rfind("as",0) == 0){ // if the file starts with "as" (AS graph)
        if(argc >= 4)
            basename = "snap_as_s"+ to_string(G.getNumVertices()) + ".A" + argv[3];
    }

    // Construct the output file path
    string output_file = output_folder + "/" + basename;

    if (G.getNumVertices() < MIN_NODES) {
        std::cout << "Graph has less than " << MIN_NODES << " nodes, skipping" << std::endl;
        return 0;
    }

    if (SAMPLING && G.getNumVertices() > N) {
        // sample graph twice to create pair

        int E = int(N * CONNECTIVITY);
        // Seed the random number generator with the current time
        std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());
        // Define the normal distribution with mean 0 and standard deviation 1
        std::normal_distribution<double> G_distribution(0.0, OFFSET_STDEV * float(N) / 100);
        std::normal_distribution<double> E_distribution(0.0, OFFSET_STDEV * float(E) / 100);
        // Sample the two graphs
        Graph G0 = G.sample(N + int(G_distribution(generator)), E + int(E_distribution(generator)));
        Graph G1 = G.sample(N + int(G_distribution(generator)), E + int(E_distribution(generator)));

        // Save the graphs
        string output_folder = output_file + "_N-" + to_string(N) + "_Conn-" + to_string(int(round(
                float(G0.getNumEdges() + G1.getNumEdges()) / float(G0.getNumVertices() + G1.getNumVertices()))));
        mkdir(output_folder.c_str(), 0777);
        std::cout << "Saving " << output_folder << "/g1.txt" << std::endl;
        saveGraphToFile(G0, output_folder + "/g1.txt");
        std::cout << "Saving " << output_folder << "/g2.txt" << std::endl;
        saveGraphToFile(G1, output_folder + "/g2.txt");
    } else if (GENERATE_SYNTHETIC_PAIR) {
        Graph G0 = G;
        Graph G1 = G;

        // add edges to both graphs
        std::cout << "Adding edges to both graphs" << std::endl;
        for (int i = 0; i < G.getNumVertices(); i++) {
            for (int j = i + 1; j < G.getNumVertices(); j++) {
                // has ADDED_EDGES_PROB probability of adding an edge
                if (!G.get(i, j))
                    if (rand() % 10000 < ADDED_EDGES_PROB)
                        G0.addEdge(i, j);
                    else if (rand() % 10000 < ADDED_EDGES_PROB)
                        G1.addEdge(i, j);
            }
        }

        // add nodes to both graphs
        std::cout << "Adding nodes to both graphs" << std::endl;
        for (int i = 0; i < G.getNumVertices(); i++) {
            if (rand() % 100 < ADDED_NODES_PROB) {
                int new_n = G0.addNode();
                G0.addEdge(i, new_n);
                for (int j = 0; j < G0.getNumVertices() - 1; j++)
                    if (j != i && rand() % 10000 < ADDED_EDGES_PROB)
                        G0.addEdge(new_n, j);
            } else if (rand() % 100 < ADDED_NODES_PROB) {
                int new_n = G1.addNode();
                G1.addEdge(i, new_n);
                for (int j = 0; j < G1.getNumVertices() - 1; j++)
                    if (j != i && rand() % 10000 < ADDED_EDGES_PROB)
                        G1.addEdge(i, j);
            }
        }


        // Save the graphs
        string output_folder = output_file;
        mkdir(output_folder.c_str(), 0777);
        std::cout << "Saving " << output_folder << "/g1.txt" << std::endl;
        saveGraphToFile(G0, output_folder + "/g1.txt");
        std::cout << "Saving " << output_folder << "/g2.txt" << std::endl;
        saveGraphToFile(G1, output_folder + "/g2.txt");
        // save size of G to file
        std::cout << "Saving " << output_folder << "/solution.txt" << std::endl;
        std::ofstream outfile;
        outfile.open(output_folder + "/solution.txt");
        outfile << G.getNumVertices() << " " << G.getNumEdges() << endl;
        outfile.close();
    } else {
// write original graph
        std::cout << "Saving " << output_file <<
                  std::endl;
        saveGraphToFile(G, output_file
                           + ".txt");
    }

    return 0;
}