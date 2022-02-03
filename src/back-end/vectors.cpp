#include <iostream>
#include <vector>

using namespace std;

// first element is dist and second is id
class priorityQueue
{
    private:
    vector<pair <int,int>> vect;

    public:
    bool empty()
    {
        return vect.empty();
    }

    int getMin() 
    {
        return vect[0].second;
    }

    void del() 
    {
        if (!vect.empty())
        {
            vect.erase(vect.begin());
        }
    }

    void ins(int dist, int id)
    {
        int i = 0;
        if (!vect.empty())
        {
            while (vect[i].first < dist)
            {
                i++;
            }
            vector<pair <int,int>>::iterator it;
            it = vect.begin();
            vect.insert(it + i, make_pair(dist, id));
        }
        else
        {
            vector<pair <int,int>>::iterator it;
            it = vect.begin();
            vect.insert(it, make_pair(dist, id));
        }
        
    }
};