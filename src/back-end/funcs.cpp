#include "headers.h"
#include <vector>
#include <iostream>
#include <tuple>
#include <climits>
#include <cmath>
#include <algorithm>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/writer.h>
#include <fstream>

// Reads N (number of objects that will follow)
void readN(int &n, FILE *f)
{
    fread(&n, sizeof(int), 1, f);
}

// Reads info of a node
void readNode(int &id, float &lat, float &lon, FILE *f)
{
    fread(&id, sizeof(int), 1, f);
    fread(&lat, sizeof(float), 1, f);
    fread(&lon, sizeof(float), 1, f);
}

// Reads info of an edge
void readEdge(int &id, float &len, FILE *f)
{
    fread(&id, sizeof(int), 1, f);
    fread(&len, sizeof(float), 1, f);
}

// Makes a node from binary file
void makeNode(Node *&node, FILE *f)
{
    int id;
    float lat, lon;
    readNode(id, lat, lon, f);
    node = new Node(id, lat, lon);
}

// Makes an edge from binary file
void makeEdge(Edge *&edge, std::vector<Node *> &nodes, FILE *f)
{
    int id;
    float len;
    readEdge(id, len, f);
    edge = new Edge(nodes[id], len);
}

// Reads binary file's content into given vector
std::tuple<int, int> readFile(std::vector<Node *> &nodes, std::string path)
{

    // ######## Opening File ########
    FILE *f;

    f = fopen(path.c_str(), "rb");
    if (!f)
    {
        std::cout << "File could not be opened\n";
        exit(1);
    }

    // ######## Reading Nodes ########
    int number_of_nodes;
    readN(number_of_nodes, f);

    for (int i = 0; i < number_of_nodes; i++)
    {
        Node *temp;
        makeNode(temp, f);
        nodes.push_back(temp);
    }

    // ######## Reading Edges ########
    int next;
    int current_node = -1;
    int number_of_edges = 0;

    for (int i = 0; i < number_of_nodes; i++)
    {
        // Node for which edges will be assigned to
        current_node += 1;

        readN(next, f);

        // If no edge, continue
        if (!next)
        {
            continue;
        }

        // Read edges and assing data to node
        for (int j = 0; j < next; j++)
        {
            number_of_edges += 1;
            Edge *temp;
            makeEdge(temp, nodes, f);
            nodes[current_node]->addChild(temp);
        }
    }

    fclose(f);

    return {number_of_nodes, number_of_edges};
}

// Heuristics used for shortest path
float heuristic(int a, int b, std::vector<Node *> &nodes)
{

    // Considering RJ as a flat region, each degree in lat/long
    // correspond to ~110km, or 111_000m
    float x = (nodes[a]->getLong() - nodes[b]->getLong()) * 111000;
    float y = (nodes[a]->getLat() - nodes[b]->getLat()) * 111000;
    float dist;

    // Euclidian distance
    dist = pow(x, 2) + pow(y, 2);
    dist = sqrt(dist);

    return dist;
}

std::pair<int, float> findNearestNode(float lat, float lon, std::vector<Node *> &nodes)
{

    float dist = UINT_MAX;
    float temp;
    int id;

    for (auto &node : nodes)
    {
        temp = sqrt(pow((node->getLat() - lat), 2) + pow((node->getLong() - lon), 2));

        if (temp < dist)
        {
            dist = temp;
            id = node->getId();
        }
    }

    dist = dist * 111000;

    std::pair<int, float> myPair(id, dist);

    return myPair;
}

void rebuildJson(float time, const std::vector<int> &path, const std::vector<Node *> &nodes)
{
    Json::Value root;
    Json::Value node;
    Json::StyledStreamWriter writer;

    float dist = 0;

    for (int i = 0; i < path.size(); i++)
    {

        node["long"] = nodes[path[i]]->getLong();
        node["lat"] = nodes[path[i]]->getLat();

        root["path"].append(node);

        if (i == 0)
        {
            continue;
        }

        dist += nodes[path[i - 1]]->getChild(path[i])->getLength();
    }

    root["time"].append(time);
    root["dist"].append(dist);

    std::ofstream file_id("data.json");
    writer.write(file_id, root);
}

// Shortest path
std::vector<int> shortestPath(Node *start, Node *end, std::vector<Node *> nodes, bool a_star)
{

    // Priority Queue
    Heap heap(nodes);

    // Setting up variables
    const int n = nodes.size();
    const int end_id = end->getId();

    float dist[n];
    int p[n];
    bool visited[n];

    for (int i = 0; i < n; i++)
    {
        dist[i] = __FLT_MAX__;
        visited[i] = false;
        p[i] = -1;
    }

    // Stating algorithm
    dist[start->getId()] = 0;
    heap.insert(nodes[start->getId()], 0);

    // Keep going while PQ is not empty
    while (heap.size())
    {
        // Min distance node
        Node *min = heap.getNextNode();
        int m = min->getId();
        visited[m] = true;

        // If min distance == target, break
        if (m == end->getId())
        {
            break;
        }

        // Else, add new nodes to PQ and update distance accordingly
        for (Edge *child : min->getChildren())
        {
            if (!visited[child->getTarget()->getId()])
            {

                int target_id = child->getTarget()->getId();
                float cur = dist[target_id];

                dist[target_id] = std::min(dist[m] + child->getLength(), cur);

                // If distance changed, predecessor changed as well
                if (cur != dist[target_id])
                {
                    // Insert node into PQ

                    float heuristic_sum = 0;

                    if (a_star)
                    {
                        heuristic_sum = heuristic(end_id, target_id, nodes);
                    }

                    heap.insert(nodes[target_id], dist[target_id] + heuristic_sum);
                    p[child->getTarget()->getId()] = m;
                }
            }
        }
    }

    // Reconstruct path
    std::vector<int> path;

    int j = end->getId();
    path.push_back(end->getId());

    while (j != start->getId())
    {

        path.push_back(p[j]);
        j = p[j];
    }

    std::reverse(path.begin(), path.end());

    return path;
}