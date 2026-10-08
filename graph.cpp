#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <utility>
#include <algorithm>
#include <stdexcept>

using namespace std;

class Node {
public:
    string name;

    Node(const string& n) : name(n) {}
};

class Edge {
public:
    int source;
    int target;
    int weight;
    bool directed;

    Edge(int s, int t, int w, bool d)
        : source(s), target(t), weight(w), directed(d) {}

    pair<int, int> key() const {
        if (directed)
            return make_pair(source, target);
        return make_pair(min(source, target), max(source, target));
    }
};

class Graph {
private:
    vector<Node> nodes;
    vector<Edge> edges;
    bool directed;

    int indexOf(const string& name) const {
        for (size_t i = 0; i < nodes.size(); i++)
            if (nodes[i].name == name) return (int)i;
        throw invalid_argument("Node not found: " + name);
    }

public:
    Graph(const vector<vector<int> >& matrix) : directed(false) {
        int n = matrix.size();

        for (int i = 0; i < n; i++)
            if ((int)matrix[i].size() != n)
                throw invalid_argument("Adjacency matrix must be square (n x n)");

        for (int i = 0; i < n; i++)
            nodes.push_back(Node(string(1, (char)('A' + i))));

        for (int i = 0; i < n && !directed; i++)
            for (int j = 0; j < n; j++)
                if (matrix[i][j] != matrix[j][i]) {
                    directed = true;
                    break;
                }

        for (int i = 0; i < n; i++) {
            int start = directed ? 0 : i;
            for (int j = start; j < n; j++) {
                if (matrix[i][j] != 0)
                    edges.push_back(Edge(i, j, matrix[i][j], directed));
            }
        }
    }

    void addEdge(const string& src, const string& dst, int weight = 1) {
        edges.push_back(Edge(indexOf(src), indexOf(dst), weight, directed));
    }

    bool isMultigraph() const {
        set<pair<int, int> > seen;
        for (size_t k = 0; k < edges.size(); k++) {
            pair<int, int> key = edges[k].key();
            if (seen.count(key)) return true;
            seen.insert(key);
        }
        return false;
    }

    bool isPseudograph() const {
        for (size_t k = 0; k < edges.size(); k++)
            if (edges[k].source == edges[k].target) return true;
        return false;
    }

    bool isDigraph() const {
        return directed;
    }

    bool isWeighted() const {
        for (size_t k = 0; k < edges.size(); k++)
            if (edges[k].weight != 1) return true;
        return false;
    }

    bool isComplete() const {
        set<pair<int, int> > pairs;
        for (size_t k = 0; k < edges.size(); k++) {
            pairs.insert(make_pair(edges[k].source, edges[k].target));
            if (!directed)
                pairs.insert(make_pair(edges[k].target, edges[k].source));
        }
        int n = nodes.size();
        for (int u = 0; u < n; u++)
            for (int v = 0; v < n; v++)
                if (u != v && !pairs.count(make_pair(u, v)))
                    return false;
        return true;
    }

    vector<string> isolatedNodes() const {
        vector<bool> connected(nodes.size(), false);
        for (size_t k = 0; k < edges.size(); k++) {
            if (edges[k].source != edges[k].target) {
                connected[edges[k].source] = true;
                connected[edges[k].target] = true;
            }
        }
        vector<string> result;
        for (size_t i = 0; i < nodes.size(); i++)
            if (!connected[i]) result.push_back(nodes[i].name);
        return result;
    }

    bool isDisjointed() const {
        return !isolatedNodes().empty();
    }

    void report() const {
        cout << boolalpha;

        cout << "Nodes (" << nodes.size() << "): ";
        for (size_t i = 0; i < nodes.size(); i++)
            cout << nodes[i].name << " ";
        cout << "\n";

        cout << "Edges (" << edges.size() << "):\n";
        for (size_t k = 0; k < edges.size(); k++) {
            const Edge& e = edges[k];
            cout << "  " << nodes[e.source].name
                 << (e.directed ? " -> " : " -- ")
                 << nodes[e.target].name
                 << "  (w=" << e.weight << ")\n";
        }

        cout << endl << "Multigraph  : " << isMultigraph()  << "\n";
        cout << "Pseudograph : " << isPseudograph() << "\n";
        cout << "Digraph     : " << isDigraph()     << "\n";
        cout << "Weighted    : " << isWeighted()    << "\n";
        cout << "Complete    : " << isComplete()    << "\n";
        cout << "Disjointed  : " << isDisjointed()  << "  (isolated: ";
        vector<string> iso = isolatedNodes();
        for (size_t i = 0; i < iso.size(); i++) cout << iso[i] << " ";
        cout << ")\n";
    }
};

int main() {
    vector<vector<int> > matrix = {
        {0,  2,  1,  7,  0,  0,  0,  0,  0},
        {2,  0,  5,  5,  0,  0,  0,  0,  0},
        {1,  5,  0,  4,  0,  9,  0,  0,  0},
        {7,  5,  4,  0,  8,  0,  0,  0,  0},
        {0,  0,  0,  8,  0,  3,  7,  0,  0},
        {0,  0,  9,  0,  3,  0,  5, 10,  0},
        {0,  0,  0,  0,  7,  5,  0, 11,  6},
        {0,  0,  0,  0,  0, 10, 11,  0,  3},
        {0,  0,  0,  0,  0,  0,  6,  3,  0}
    };

    cout << "SAMPLE GRAPH\n";
    Graph g(matrix);
    g.report();

    cout << boolalpha;

    cout << "\nTest: parallel edge + loop\n";
    Graph g2(matrix);
    g2.addEdge("A", "B", 9);   // edge ขนาน -> multigraph
    g2.addEdge("C", "C", 1);   // loop      -> pseudograph
    cout << "Multigraph  : " << g2.isMultigraph()  << "\n";
    cout << "Pseudograph : " << g2.isPseudograph() << "\n";

    cout << "\nTest: directed\n";
    Graph g3({{0, 1, 0},
              {0, 0, 1},
              {1, 0, 0}});
    cout << "Digraph     : " << g3.isDigraph() << "\n";

    cout << "\nTest: complete K3\n";
    Graph g4({{0, 1, 1},
              {1, 0, 1},
              {1, 1, 0}});
    cout << "Complete    : " << g4.isComplete() << "\n";
    cout << "Weighted    : " << g4.isWeighted() << "\n";

    cout << "\nTest: isolated node\n";
    Graph g5({{0, 2, 0},
              {2, 0, 0},
              {0, 0, 0}});
    cout << "Disjointed  : " << g5.isDisjointed() << "\n";

    return 0;
}