#include <iostream>
#include "Rtree.h"

int main() {
    RTree tree;

    // Вставляем точки
    tree.insert({ 2.5, 3.0 });
    tree.insert({ 1.0, 4.0 });
    tree.insert({ 1.5, 2.5 });
    tree.insert({ 3.5, 2.0 });
    tree.insert({ 4.0, 3.5 });
    Node* root = tree.root;
    //диапазонный поиск
    vector<float> center = { 2.0, 3.0 };
    float radius = 2.0;
    auto inRegion = tree.searchRegion(center, radius);
    cout << "Points within radius " << radius << " of (" << center[0] << ", " << center[1] << "):" << endl;
    for (auto& point : inRegion) {
        cout << "(" << point[0] << ", " << point[1] << ")" << endl;
    }
    //ближайшая точка к заданной
    vector<float> point = { 2.0, 3.0 };
    vector<float> nearestPoint = tree.search(point);
    cout << "Nearest point to (" << point[0] << ", " << point[1] << ") is (" << nearestPoint[0] << ", " << nearestPoint[1] << ")" << endl;
    //поиск соседа
    vector<float> neighbor = { 3.0, 4.0 };
    vector<float> nearestNeighbor = tree.findNearestNeighbor(root, neighbor);
    cout << "Nearest neighbor to (" << neighbor[0] << ", " << neighbor[1] << ") is (" << nearestNeighbor[0] << ", " << nearestNeighbor[1] << ")" << endl;
    // Удаляем точку
    tree.remove({ 2.5, 3.0 });

    return 0;
}