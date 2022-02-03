#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include <chrono>

#ifndef STRUCTS
#define STRUCTS

/* Classes */

class Edge;

class Node
{
private:
    int id;
    float lat, lon;
    std::vector<Edge *> children;

public:
    Node(int i, float _lat, float _lon) : id(i), lat(_lat), lon(_lon){};
    Node(){};

    void addChild(Edge *edge);

    int getId() { return id; };
    float getLat() { return lat; };
    float getLong() { return lon; };
    Edge *getChild(int id);
    std::vector<Edge *> getChildren() { return children; }
};

class Edge
{
private:
    float length;
    Node *target;

public:
    Edge(Node *t, float len) : length(len), target(t) {}
    Edge(){};

    float getLength() { return length; };
    Node *getTarget() { return target; };
};

class Heap
{
public:
    Heap(std::vector<Node *> &_nodes) : nodes(_nodes){};
    int size();
    void insert(Node *node, float key);
    std::pair<int, float> extract_min();
    Node *getNextNode();

private:
    std::vector<std::pair<int, float>> data;
    std::vector<Node *> nodes;

    void swap(std::pair<int, float> *a, std::pair<int, float> *b);
    long unsigned int get_right_child(long unsigned int index);
    long unsigned int get_left_child(long unsigned int index);
    long unsigned int get_parent(long unsigned int index);
    void min_heapify(long unsigned int index);
    void decrease_key(long unsigned int index, float key);
};

/* Funções  */

void readN(int &n, FILE *f);
void readNode(int &id, float &lat, float &lon, FILE *f);
void readEdge(int &id, float &len, FILE *f);
void makeNode(Node *&node, FILE *f);
void makeEdge(Edge *&edge, std::vector<Node *> &nodes, FILE *f);
std::tuple<int, int> readFile(std::vector<Node *> &nodes, std::string path);
int length(int s, int t, std::vector<Node *> nodes);
std::vector<int> shortestPath(Node *start, Node *end, std::vector<Node *> nodes, bool a_star);
std::pair<int, float> findNearestNode(float lat, float lon, std::vector<Node *> &nodes);
void rebuildJson(float time, const std::vector<int> &path, const std::vector<Node *> &nodes);
float heuristic(int a, int b, std::vector<Node *> &nodes);

/* Servidor */

MHD_Result initial_page(std::string url, struct MHD_Connection *connection);
MHD_Result error_page(struct MHD_Connection *connection);
MHD_Response *json(std::string url, struct MHD_Connection *connection);
MHD_Result answer_to_connection(void *cls, struct MHD_Connection *connection,
                                const char *url,
                                const char *method, const char *version,
                                const char *upload_data,
                                size_t *upload_data_size, void **con_cls);

#endif