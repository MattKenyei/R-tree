#include "Rtree.h"

vector<vector<float>> RTree::searchRegionHelper(Node* node, vector<float> point, float radius) const
{
    vector<vector<float>> result;

    if (node->isLeaf) {
        float distance = node->calculateDistance(point);
        if (distance <= radius) {
            result.push_back(node->coordinates);
        }
    }
    else {
        for (auto& child : node->children) {
            float distance = child->calculateDistance(point);
            if (distance <= radius) {
                auto childResult = searchRegionHelper(child, point, radius);
                result.insert(result.end(), childResult.begin(), childResult.end());
            }
        }
    }

    return result;
}

Node* RTree::findNearestNeighborHelper(Node* node, vector<float> point, Node* bestNode) const
{
    float distance = node->calculateDistance(point);
    float bestDistance = numeric_limits<float>::max();

    if (bestNode) bestDistance = bestNode->calculateDistance(point);

    if (distance < bestDistance && !node->isLeaf) {
        for (auto& child : node->children) {
            Node* closestChild = findNearestNeighborHelper(child, point, bestNode);
            float childDistance = closestChild->calculateDistance(point);

            if (childDistance < bestDistance) {
                bestNode = closestChild;
                bestDistance = childDistance;
            }
        }
    }
    else if (distance < bestDistance && node->isLeaf) {
        bestNode = node;
        bestDistance = distance;
    }

    return bestNode;
}

vector<float> RTree::searchHelper(Node* node, vector<float> point) const
{
    if (node->isLeaf) return node->coordinates;

    float minDistance = node->calculateDistance(point);
    Node* closestChild = nullptr;

    for (auto& child : node->children) {
        float distance = child->calculateDistance(point);
        if (distance < minDistance) {
            minDistance = distance;
            closestChild = child;
        }
    }

    if (closestChild) return searchHelper(closestChild, point);
    else return node->coordinates;
}

void RTree::insertHelper(Node* node, vector<float> point)
{
    if (node->isLeaf) {
        if (!node->isFull()) node->addChild(new Node(point, true));
        else splitNode(node, point);
    }
    else {
        float minDistance = node->calculateDistance(point);
        Node* closestChild = nullptr;

        for (auto& child : node->children) {
            float distance = child->calculateDistance(point);
            if (distance < minDistance) {
                minDistance = distance;
                closestChild = child;
            }
        }

        insertHelper(closestChild, point);
    }
}

void RTree::splitNode(Node* node, vector<float> point)
{
    Node* newNode = new Node();
    node->isLeaf = false;
    newNode->isLeaf = true;

    int splitIndex = chooseSplitIndex(node, point);
    vector<Node*> childrenCopy = node->children;
    node->children.clear();

    for (auto& child : childrenCopy) {
        if (node->children.size() < splitIndex) node->addChild(child);
        else newNode->addChild(child);
    }

    node->coordinates = calculateNodeCoordinates(node);
    newNode->coordinates = calculateNodeCoordinates(newNode);

    if (node == root) {
        root = new Node(calculateNodeCoordinates(node), false);
        root->addChild(node);
    }

    root->addChild(newNode);

    insertHelper(node, point);
}

int RTree::chooseSplitIndex(Node* node, vector<float> point) const
{
    int bestIndex = -1;
    float bestDistance = numeric_limits<float>::max();

    for (int i = 0; i < node->children.size(); i++) {
        float distance = node->children[i]->calculateDistance(point);
        if (distance < bestDistance) {
            bestIndex = i;
            bestDistance = distance;
        }
    }

    return bestIndex + 1;
}
vector<float> RTree::calculateNodeCoordinates(Node* node) const
{
    vector<float> sum(node->coordinates.size(), 0);
    for (auto& child : node->children) {
        for (int i = 0; i < child->coordinates.size(); i++)
        {
            sum[i] += child->coordinates[i];
        }
    }
    for (int i = 0; i < sum.size(); i++) {
        sum[i] /= node->children.size();
    }
    return sum;
}

void RTree::removeHelper(Node* node, vector<float> point)
{
    if (node->isLeaf)
        node->removeChild(new Node(point, true));
    else {
        float minDistance = node->calculateDistance(point);
        Node* closestChild = nullptr;

        for (auto& child : node->children) {
            float distance = child->calculateDistance(point);
            if (distance < minDistance) {
                minDistance = distance;
                closestChild = child;
            }
        }

        removeHelper(closestChild, point);

        if (closestChild->children.empty() && node->children.size() > MIN_CHILDREN) {
            node->removeChild(closestChild);
            node->coordinates = calculateNodeCoordinates(node);
        }
        else if (closestChild->children.empty() && node != root) {
            mergeNodes(node, closestChild);
        }
    }
}
void RTree::mergeNodes(Node* parent, Node* child)
{
    Node* sibling = findSibling(parent, child);
    vector<Node*> mergedChildren = child->children;

    if (sibling->isFull()) {
        splitNode(sibling, calculateNodeCoordinates(child));
        for (int i = 0; i < child->children.size(); i++) {
            if (i < MIN_CHILDREN) sibling->addChild(child->children[i]);
            else sibling->children.back()->addChild(child->children[i]);
        }
        sibling->coordinates = calculateNodeCoordinates(sibling);
        parent->removeChild(child);
    }
    else {
        for (auto& grandchild : mergedChildren) {
            sibling->addChild(grandchild);
        }
        sibling->coordinates = calculateNodeCoordinates(sibling);
        parent->removeChild(child);
        if (parent == root && parent->children.size() == 1) {
            root = sibling;
            delete parent;
        }
        else if (parent != root && parent->children.size() < MIN_CHILDREN) {
            mergeNodes(getParent(root, parent), parent);
        }
    }
}
Node* RTree::findSibling(Node* parent, Node* node) const
{
    for (int i = 0; i < parent->children.size(); i++)
    {
        if (parent->children[i] != node && !parent->children[i]->isLeaf)
        {
            for (auto& child : parent->children[i]->children)
            {
                if (child == node) return parent->children[i];
            }
        }
    }

    return nullptr;
}

Node* RTree::getParent(Node* node, Node* child) const
{
    if (node->children.empty()) return nullptr;

    for (auto& n : node->children) {
        if (n == child) return node;
        else if (!n->isLeaf) {
            Node* parent = getParent(n, child);
            if (parent) return parent;
        }
    }

    return nullptr;
}
vector<float> RTree::search(vector<float> point) const
{
	return searchHelper(root, point);
}

void RTree::insert(vector<float> point)
{
	if (!root) 
		root = new Node(point, true);
	else 
		insertHelper(root, point);
}

void RTree::remove(vector<float> point)
{
	removeHelper(root, point);
}

vector<float> RTree::findNearestNeighbor(Node* node, vector<float> point) const
{
	Node* nearestNode = findNearestNeighborHelper(node, point, nullptr);
	return nearestNode->coordinates;
}

vector<vector<float>> RTree::searchRegion(vector<float> point, float radius) const
{
	return searchRegionHelper(root, point, radius);
}
