#include "headers.h"
#include <tuple>

int main(int argc, char *argv[])
{
    int source, target;
    std::vector<Node *> nodes;

    source = std::stoi(argv[1]), target = std::stoi(argv[2]);
    std::tuple<int, int> values = readFile(nodes, "../../data/RJ");

    // std::cout << std::get<0>(values) << " " << std::get<1>(values) << std::endl;

    std::pair<std::vector<int>, std::vector<bool>> caminho;

    std::cout << std::stoi(argv[3]) << std::endl;
    caminho = shortestPath(nodes[source], nodes[target], nodes, std::stoi(argv[3]));
}