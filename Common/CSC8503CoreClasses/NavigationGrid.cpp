#include "NavigationGrid.h"
#include "Assets.h"

#include <fstream>
#include <vector>

using namespace NCL;
using namespace CSC8503;

const int LEFT_NODE = 0;
const int RIGHT_NODE = 1;
const int TOP_NODE = 2;
const int BOTTOM_NODE = 3;

const char WALL_NODE = 'x';
const char FLOOR_NODE = '.';

NavigationGrid::NavigationGrid() {
    nodeSize = 0;
    gridWidth = 0;
    gridHeight = 0;
    allNodes = nullptr;
}

NavigationGrid::NavigationGrid(const std::string &filename) : NavigationGrid() {
    std::ifstream infile(Assets::DATADIR + filename);

    infile >> nodeSize;
    infile >> gridWidth;
    infile >> gridHeight;

    allNodes = new GridNode[gridWidth * gridHeight];

    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            GridNode& n = allNodes[(gridWidth * y) + x];
            char type = 0;
            infile >> type;
            if (type == '1')n.type = 12345;
            if (type == '2')n.type = 10086;
            if (type == '3')n.type = 10010;
            if (type == '4')n.type = 10000;
            if (type == '5' || type == '7')n.type = 114514;
            if (type == '6')n.type = 7;
            if (type == '0')n.type = 0;
            n.position = Vector3((float) (x * nodeSize), 7, (float) (y * nodeSize));
        }
    }

    //now to build the connectivity between the nodes
    for (int y = 0; y < gridHeight; ++y) {
        for (int x = 0; x < gridWidth; ++x) {
            GridNode &n = allNodes[(gridWidth * y) + x];

            if (y > 0) { //get the above node
                n.connected[0] = &allNodes[(gridWidth * (y - 1)) + x];
            }
            if (y < gridHeight - 1) { //get the below node
                n.connected[1] = &allNodes[(gridWidth * (y + 1)) + x];
            }
            if (x > 0) { //get left node
                n.connected[2] = &allNodes[(gridWidth * (y)) + (x - 1)];
            }
            if (x < gridWidth - 1) { //get right node
                n.connected[3] = &allNodes[(gridWidth * (y)) + (x + 1)];
            }
            for (int i = 0; i < 4; ++i) {
                if (n.connected[i]) {
                    if (n.connected[i]->type == 0 || n.connected[i]->type == 7) {
                        n.costs[i] = 1;
                    }
                    else {
                        n.costs[i] = 10000;
                    }
                    //if (n.connected[i]->type == 'x') {
                    //    n.connected[i] = nullptr; //actually a wall, disconnect!
                    //}
                }
            }
        }
    }
}

NavigationGrid::~NavigationGrid() {
    delete[] allNodes;
}

bool NavigationGrid::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
    //need to work out which node 'from' sits in, and 'to' sits in
    int fromX = ((int)from.x / nodeSize);
    int fromZ = ((int)from.z / nodeSize);

    int toX = ((int)to.x / nodeSize);
    int toZ = ((int)to.z / nodeSize);

    if (fromX < 0 || fromX > gridWidth - 1 ||
        fromZ < 0 || fromZ > gridHeight - 1) {
        return false; //outside of map region!
    }

    if (toX < 0 || toX > gridWidth - 1 ||
        toZ < 0 || toZ > gridHeight - 1) {
        return false; //outside of map region!
    }

    GridNode* startNode = &allNodes[(fromZ * gridWidth) + fromX];
    GridNode* endNode = &allNodes[(toZ * gridWidth) + toX];

    std::vector<GridNode*> openList;
    std::vector<GridNode*> closedList;

    openList.push_back(startNode);

    startNode->f = 0;
    startNode->g = 0;
    startNode->parent = nullptr;

    GridNode* currentBestNode = nullptr;

    while (!openList.empty()) {
        currentBestNode = RemoveBestNode(openList);

        if (currentBestNode == endNode) {            //we've found the path!
            GridNode* node = endNode;
            while (node != nullptr) {
                outPath.PushWaypoint(node->position);
                node = node->parent;
            }
            return true;
        }
        else {
            for (int i = 0; i < 4; ++i) {
                GridNode* neighbour = currentBestNode->connected[i];
                //if (!neighbour || (neighbour != endNode && (neighbour->type != 0 && neighbour->type != 7))) { //might not be connected...
                if (!neighbour) { //might not be connected...
                    continue;
                }
                bool inClosed = NodeInList(neighbour, closedList);
                if (inClosed) {
                    continue; //already discarded this neighbour...
                }

                float h = Heuristic(neighbour, endNode);
                float g = currentBestNode->g + currentBestNode->costs[i];
                float f = h + g;

                bool inOpen = NodeInList(neighbour, openList);

                if (!inOpen) { //first time we've seen this neighbour
                    openList.emplace_back(neighbour);
                }
                //todo:inOpen?&&
                if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
                    neighbour->parent = currentBestNode;
                    neighbour->f = f;
                    neighbour->g = g;
                }
            }
            closedList.emplace_back(currentBestNode);
        }
    }
    return false; //open list emptied out with no path!
}

bool NavigationGrid::FindPath2(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
    //need to work out which node 'from' sits in, and 'to' sits in
    int fromX = ((int)from.x / nodeSize);
    int fromZ = ((int)from.z / nodeSize);

    int toX = ((int)to.x / nodeSize);
    int toZ = ((int)to.z / nodeSize);

    if (fromX < 0 || fromX > gridWidth - 1 ||
        fromZ < 0 || fromZ > gridHeight - 1) {
        return false; //outside of map region!
    }

    if (toX < 0 || toX > gridWidth - 1 ||
        toZ < 0 || toZ > gridHeight - 1) {
        return false; //outside of map region!
    }

    GridNode* startNode = &allNodes[(fromZ * gridWidth) + fromX];
    GridNode* endNode = &allNodes[(toZ * gridWidth) + toX];

    std::vector<GridNode*> openList;
    std::vector<GridNode*> closedList;

    openList.push_back(startNode);

    startNode->f = 0;
    startNode->g = 0;
    startNode->parent = nullptr;

    GridNode* currentBestNode = nullptr;

    while (!openList.empty()) {
        currentBestNode = RemoveBestNode(openList);

        if (currentBestNode == endNode) {            //we've found the path!
            GridNode* node = endNode;
            while (node != nullptr) {
                outPath.PushWaypoint(node->position);
                node = node->parent;
            }
            return true;
        }
        else {
            for (int i = 0; i < 4; ++i) {
                GridNode* neighbour = currentBestNode->connected[i];
                if (!neighbour || (neighbour != endNode && (neighbour->type != 0 && neighbour->type != 7))) { //might not be connected...
                    continue;
                }
                bool inClosed = NodeInList(neighbour, closedList);
                if (inClosed) {
                    continue; //already discarded this neighbour...
                }

                float h = Heuristic(neighbour, endNode);
                float g = currentBestNode->g + currentBestNode->costs[i];
                float f = h + g;

                bool inOpen = NodeInList(neighbour, openList);

                if (!inOpen) { //first time we've seen this neighbour
                    openList.emplace_back(neighbour);
                }
                //todo:inOpen?&&
                if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
                    neighbour->parent = currentBestNode;
                    neighbour->f = f;
                    neighbour->g = g;
                }
            }
            closedList.emplace_back(currentBestNode);
        }
    }
    return false; //open list emptied out with no path!
}

bool NavigationGrid::FindPath(const Vector3& from, const Vector3& to, float& outF) {
    //need to work out which node 'from' sits in, and 'to' sits in
    int fromX = ((int)from.x / nodeSize);
    int fromZ = ((int)from.z / nodeSize);

    int toX = ((int)to.x / nodeSize);
    int toZ = ((int)to.z / nodeSize);

    if (fromX < 0 || fromX > gridWidth - 1 ||
        fromZ < 0 || fromZ > gridHeight - 1) {
        return false; //outside of map region!
    }

    if (toX < 0 || toX > gridWidth - 1 ||
        toZ < 0 || toZ > gridHeight - 1) {
        return false; //outside of map region!
    }

    GridNode* startNode = &allNodes[(fromZ * gridWidth) + fromX];
    GridNode* endNode = &allNodes[(toZ * gridWidth) + toX];

    std::vector<GridNode*> openList;
    std::vector<GridNode*> closedList;

    openList.push_back(startNode);

    startNode->f = 0;
    startNode->g = 0;
    startNode->parent = nullptr;

    GridNode* currentBestNode = nullptr;

    while (!openList.empty()) {
        currentBestNode = RemoveBestNode(openList);

        if (currentBestNode == endNode) {            //we've found the path!
            GridNode* node = endNode;
            while (node != nullptr) {
                if (node->parent && node->parent == startNode) {
                    outF = node->f;
                }
                node = node->parent;
            }
            return true;
        }
        else {
            for (int i = 0; i < 4; ++i) {
                GridNode* neighbour = currentBestNode->connected[i];
                //if (!neighbour || (neighbour != endNode && (neighbour->type != 0 && neighbour->type != 7))) { //might not be connected...
                if (!neighbour) { //might not be connected...
                    continue;
                }
                bool inClosed = NodeInList(neighbour, closedList);
                if (inClosed) {
                    continue; //already discarded this neighbour...
                }

                float h = Heuristic(neighbour, endNode);
                float g = currentBestNode->g + currentBestNode->costs[i];
                float f = h + g;

                bool inOpen = NodeInList(neighbour, openList);

                if (!inOpen) { //first time we've seen this neighbour
                    openList.emplace_back(neighbour);
                }
                //todo:inOpen?&&
                if (!inOpen || f < neighbour->f) {//might be a better route to this neighbour
                    neighbour->parent = currentBestNode;
                    neighbour->f = f;
                    neighbour->g = g;
                }
            }
            closedList.emplace_back(currentBestNode);
        }
    }
    return false; //open list emptied out with no path!
}

bool NavigationGrid::NodeInList(GridNode *n, std::vector<GridNode *> &list) const {
    std::vector<GridNode *>::iterator i = std::find(list.begin(), list.end(), n);
    return i == list.end() ? false : true;
}

GridNode *NavigationGrid::RemoveBestNode(std::vector<GridNode *> &list) const {
    std::vector<GridNode *>::iterator bestI = list.begin();

    GridNode *bestNode = *list.begin();

    for (auto i = list.begin(); i != list.end(); ++i) {
        if ((*i)->f < bestNode->f) {
            bestNode = (*i);
            bestI = i;
        }
    }
    list.erase(bestI);

    return bestNode;
}

float NavigationGrid::Heuristic(GridNode *hNode, GridNode *endNode) const {
    return (hNode->position - endNode->position).Length();
}

Vector3 NavigationGrid::FindNearestTree(const Vector3& position) {
    std::vector<float> fs;
    for (int i = 0; i < gridWidth * gridHeight; ++i) {
        if (allNodes[i].type == 10086 && (allNodes[i].position - position).Length() < 43) {
            float f = 50000.0f;
            FindPath(position, allNodes[i].position, f);
            fs.push_back(f);
        }
        else
            fs.push_back(50000.0f);
    }

    std::vector<float>::iterator smallest = std::min_element(fs.begin(), fs.end());
    int min_number = *smallest;
    int index = std::distance(fs.begin(), smallest);

    return min_number != 50000.0f ? allNodes[index].position : position;
}

Vector3 NavigationGrid::FindNearestRock(const Vector3& position) {
    std::vector<float> fs;
    for (int i = 0; i < gridWidth * gridHeight; ++i) {
        if (allNodes[i].type == 10010 && (allNodes[i].position - position).Length() < 43) {
            float f = 50000.0f;
            FindPath(position, allNodes[i].position, f);
            fs.push_back(f);
        }
        else
            fs.push_back(50000.0f);
    }

    std::vector<float>::iterator smallest = std::min_element(fs.begin(), fs.end());
    int min_number = *smallest;
    int index = std::distance(fs.begin(), smallest);

    return min_number != 50000.0f ? allNodes[index].position : position;
}

bool NavigationGrid::CheckInGrid(Vector3& pos) {
    int posX = ((int)pos.x / nodeSize);
    int posZ = ((int)pos.z / nodeSize);

    if (posX < 0 || posX > gridWidth - 1 ||
        posZ < 0 || posZ > gridHeight - 1) {
        return false; //outside of map region!
    }
    return true;
}