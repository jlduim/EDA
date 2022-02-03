import osmnx as ox
import re
import json
import networkx as nx


def get_json_RJ():

    g = ox.graph_from_place(
        'Rio de Janeiro', simplify=False, network_type='drive')

    data = make_JSON(g)

    with open("RJ.json", "w", encoding="utf8") as f:
        json.dump(data, f)


def get_json_gloria():

    g = ox.graph_from_address("Glória, Rio de Janeiro", network_type="drive")

    data = make_JSON(g)

    with open("gloria.json", "w", encoding="utf8") as f:
        json.dump(data, f)


def make_JSON(g):

    nodes = []
    edges = []

    edge_count = 0
    node_count = 0

    for node_id in sorted(g.nodes()):

        node_aux = g.nodes[node_id]
        nodes.append(
            {"lat": node_aux["y"], "long": node_aux["x"], "id": node_id})
        node_count += 1

        my_edges = []

        for target in g[node_id].keys():

            for info in g[node_id][target].values():

                my_edges.append({"id": target, "length": info["length"]})
                edge_count += 1

        edges.append(my_edges.copy())

    print("Nós escritos: ", node_count)
    print("Arestas escritas: ", edge_count)

    return {"nodes": nodes, "edges": edges}


if __name__ == "__main__":
    # get_json_gloria()
    get_json_RJ()
