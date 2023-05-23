#pragma once
#include <vector>
#include <algorithm>

using namespace std;

const int MAX_CHILDREN = 4;
const int MIN_CHILDREN = 2;

struct Node {
    vector<float> coordinates;
    bool isLeaf;
    vector<Node*> children;

    Node() {}
    Node(vector<float> coords, bool leaf) : coordinates(coords), isLeaf(leaf) {}

    float calculateDistance(vector<float> point) const {
        float sum = 0;
        for (int i = 0; i < coordinates.size(); i++) {
            sum += pow(coordinates[i] - point[i], 2);
        }
        return sqrt(sum);
    }

    void addChild(Node* child)
    {
        children.push_back(child);
    }
    void removeChild(Node* child)
    {
        auto it = find(children.begin(), children.end(), child);
        if (it != children.end())
        {
            children.erase(it);
        }
    }
    bool isFull() const
    {
        return children.size() >= MAX_CHILDREN;
    }
};
class RTree 
{
private:
    vector<vector<float>> searchRegionHelper(Node* node, vector<float> point, float radius) const;

    Node* findNearestNeighborHelper(Node* node, vector<float> point, Node* bestNode) const;

    vector<float> searchHelper(Node* node, vector<float> point) const;

    void insertHelper(Node* node, vector<float> point);

    void splitNode(Node* node, vector<float> point);

    int chooseSplitIndex(Node* node, vector<float> point) const;

    vector<float> calculateNodeCoordinates(Node* node) const;
    void removeHelper(Node* node, vector<float> point);


    void mergeNodes(Node* parent, Node* child);

    Node* findSibling(Node* parent, Node* node) const;
    Node* getParent(Node* node, Node* child) const;
public:
    RTree() : root(nullptr) {}

    vector<float> search(vector<float> point) const;
    void insert(vector<float> point);
    void remove(vector<float> point);
    vector<float> findNearestNeighbor(Node* node, vector<float> point) const;

    vector<vector<float>> searchRegion(vector<float> point, float radius) const;
    Node* root;

};

