#include "headers.h"
#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <tuple>
#include <chrono>

int main()
{

    std::vector<Node *> nodes;
    std::tuple<int, int> values = readFile(nodes, "/home/joao/Documents/final_eda/final-project-grupo-1/data/RJ");

    std::cout << "Nós lidos: " << std::get<0>(values) << "\n";
    std::cout << "Arestas lidas: " << std::get<1>(values) << "\n";

    std::cout << "Qual algoritmo quer usar?" << std::endl;
    std::cout << "0. Dijkstra\n1. A*" << std::endl;

    bool use_a_star;
    std::cin >> use_a_star;

    float lat_start, long_start, lat_end, long_end;

    std::cout << "Digite a latitude e a longitude da origem, separados por espaço" << std::endl;
    std::cin >> lat_start >> long_start;

    std::cout << "Digite a latitude e a longitude do destino, separados por espaço" << std::endl;
    std::cin >> lat_end >> long_end;

    std::pair<int, float> info_start, info_end;
    int id_start, id_end;
    float dist_start, dist_end;

    info_start = findNearestNode(lat_start, long_start, nodes);
    info_end = findNearestNode(lat_end, long_end, nodes);

    id_start = info_start.first, id_end = info_end.first;
    dist_start = info_start.second, dist_end = info_end.second;

    if ((dist_start > 200) | (dist_end > 200))
    {
        std::cout << "Coordenadas muito longe do nó mais próximo" << std::endl;
        return 0;
    }

    std::vector<int> path;

    auto start = std::chrono::high_resolution_clock::now();

    path = shortestPath(nodes[id_start], nodes[id_end], nodes, use_a_star);

    auto stop = std::chrono::high_resolution_clock::now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    float tempo = time.count();

    for (auto x : path)
    {
        if (x == -1)
        {
            break;
        }
        std::cout << "Lat: " << nodes[x]->getLat() << " Long: " << nodes[x]->getLong() << std::endl;
    }
    std::cout << "\n";

    std::cout << tempo << " milisegundos" << std::endl;

    return 0;
}