#include "headers.h"
#include <iostream>
#include <jsoncpp/json/value.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <string>

// Reads JSON file
Json::Value readJson(std::string path)
{
    std::ifstream data(path, std::ifstream::binary);
    Json::Value myJSON;
    Json::Reader reader;

    reader.parse(data, myJSON);

    return myJSON;
}

// Writes the information of the node on the file
void writeNode(Json::Value &node, int ID, FILE *f)
{
    int id = ID;
    float lat = node["lat"].asFloat();
    float lon = node["long"].asFloat();
    fwrite(&id, sizeof(int), 1, f);
    fwrite(&lat, sizeof(float), 1, f);
    fwrite(&lon, sizeof(float), 1, f);
}

// Writes the information of the edge on the file
void writeEdge_(int id, float len, FILE *f)
{
    fwrite(&id, sizeof(int), 1, f);
    fwrite(&len, sizeof(float), 1, f);
}

int binarySearch(const Json::Value &nodes, int l, int r, unsigned long long x)
{

    if (r >= l)
    {

        int mid = l + (r - l) / 2;

        unsigned long long id = nodes[mid]["id"].asLargestUInt();

        if (id == x)
        {
            return mid;
        }

        if (id > x)
        {
            return binarySearch(nodes, l, mid - 1, x);
        }

        return binarySearch(nodes, mid + 1, r, x);
    }

    std::cout << "ID não encontrado: " << x << std::endl;
    return -1;
}

// Find new ID for the node
int findNewId(std::string id, const Json::Value &nodes)
{
    return binarySearch(nodes, 0, nodes.size() - 1, std::stoull(id));
}

// Writes the content of an edge to the file
void writeEdge(const Json::Value &edge, const Json::Value &nodes, int &number_of_edges_new, FILE *f)
{
    // n is the number of the following edges (necessary to keep track of what edges are from what node)
    int n = edge.size();
    number_of_edges_new += n;
    fwrite(&n, sizeof(int), 1, f);
    if (n == 0)
    {
        return;
    }

    for (int i = 0; i < n; i++)
    {
        std::string id = edge[i]["id"].asString();
        float len = edge[i]["length"].asFloat();

        int new_id = findNewId(id, nodes);
        writeEdge_(new_id, len, f);
    }
}

// Writes binary file
void toBin(std::string path)
{

    // ######## Opening File ########
    FILE *f;
    f = fopen(("../../data/" + path).c_str(), "wb");
    Json::Value data = readJson("../../data/" + path + ".json");

    // ######## Writing Nodes ########
    Json::Value nodes = data["nodes"];
    int number_of_nodes = nodes.size();
    std::cout << "Nós escritos: " << number_of_nodes << std::endl;

    fwrite(&number_of_nodes, sizeof(int), 1, f);

    for (int i = 0; i < number_of_nodes; i++)
    {
        writeNode(nodes[i], i, f);
    }

    // ######## Writing Edges ########
    Json::Value edges = data["edges"];
    int number_of_edges = 0;

    for (int i = 0; i < number_of_nodes; i++)
    {
        writeEdge(edges[i], nodes, number_of_edges, f);
    }

    std::cout << "Arestas escritas: " << number_of_edges << std::endl;

    fclose(f);
}

int main(int argc, char *argv[])
{
    toBin(argv[1]);
}