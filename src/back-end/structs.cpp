#include "headers.h"
#include <vector>
#include <iostream>
#include <climits>

void Node::addChild(Edge *edge)
{
    this->children.push_back(edge);
}

Edge *Node::getChild(int id)
{
    for (Edge *t : this->getChildren())
    {
        if (t->getTarget()->getId() == id)
        {
            return t;
        }
    }
    return NULL;
}

void Heap::swap(std::pair<int, float> *a, std::pair<int, float> *b)
{
    std::pair<int, float> t;
    t = *a;
    *a = *b;
    *b = t;
}
long unsigned int Heap::get_right_child(long unsigned int index)
{
    if (index == 0 && data.size() >= 3)
    {
        return 2;
    }

    if ((2 * index) + 1 < data.size() && index >= 0)
        return (2 * index) + 1;
    return -1;
}
long unsigned int Heap::get_left_child(long unsigned int index)
{
    if (index == 0 && data.size() >= 2)
    {
        return 1;
    }

    if ((2 * index) < data.size() && index >= 0)
        return 2 * index;
    return -1;
}
long unsigned int Heap::get_parent(long unsigned int index)
{
    if (index > 0 && index < data.size())
    {
        return index / 2;
    }
    return -1;
}
void Heap::min_heapify(long unsigned int index)
{
    long unsigned int left_child_index = get_left_child(index);
    long unsigned int right_child_index = get_right_child(index);

    long unsigned int smallest = index;

    if (left_child_index <= data.size() && left_child_index > 0)
    {
        if (data[left_child_index].second < data[smallest].second)
        {
            smallest = left_child_index;
        }
    }

    if (right_child_index <= data.size() && right_child_index > 0)
    {
        if (data[right_child_index].second < data[smallest].second)
        {
            smallest = right_child_index;
        }
    }

    if (smallest != index)
    {
        swap(&data[index], &data[smallest]);
        min_heapify(smallest);
    }
}
std::pair<int, float> Heap::extract_min()
{

    std::pair<int, float> minm = data[0];
    data[0] = data[data.size() - 1];
    min_heapify(0);
    data.pop_back();
    return minm;
}
void Heap::decrease_key(long unsigned int index, float key)
{
    data[index].second = key;
    while (index > 0 && (data[get_parent(index)].second > data[index].second))
    {
        swap(&data[index], &data[get_parent(index)]);
        index = get_parent(index);
    }
}
void Heap::insert(Node *node, float key)
{
    std::pair<int, float> t(node->getId(), __FLT_MAX__);
    data.push_back(t);
    decrease_key(data.size() - 1, key);
}

int Heap::size()
{
    return data.size();
}

Node *Heap::getNextNode()
{
    std::pair<int, float> temp = extract_min();
    int ID = temp.first;
    return nodes[ID];
}