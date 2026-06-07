// Module3.cpp (Robot Navigation and Path Tracking)
// Delfino Jeconiah Djajah - TP078795

#include "Module3.hpp"

#include <iostream>
#include <climits>

// NavNode
NavNode::NavNode(int id, const std::string& dir)
    : direction(dir), nodeID(id), distance(-1), next(nullptr) {}

NavNode::NavNode(int nodeID, int distance)
    : direction("none"), nodeID(nodeID), distance(distance), next(nullptr) {}


// LinkedList
LinkedList::LinkedList() : head(nullptr) {}

LinkedList::~LinkedList() {
    NavNode* current = head;
    while (current != nullptr) {
        NavNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
}

void LinkedList::insertHead(NavNode* node) {
    node->next = head;
    head       = node;
}

NavNode* LinkedList::removeHead() {
    if (head == nullptr) return nullptr;
    NavNode* temp = head;
    head          = head->next;
    temp->next    = nullptr;
    return temp;
}

int LinkedList::getLength() const {
    int      count = 0;
    NavNode* temp  = head;
    while (temp != nullptr) {
        count++;
        temp = temp->next;
    }
    return count;
}

void LinkedList::printList() const {
    NavNode* temp = head;
    while (temp != nullptr) {
        std::cout << temp->nodeID << "\n";
        temp = temp->next;
    }
}

void LinkedList::minInsert(NavNode* node) {
    if (node->distance == -1) return; 

    if (head == nullptr) {
        head = node;
        return;
    }

    NavNode* temp   = head;
    NavNode* prev   = head;
    bool     isHead = true;

    while (temp != nullptr) {
        if (temp->distance > node->distance) {
            if (isHead) {
                node->next = temp;
                head       = node;
                return;
            } else {
                prev->next = node;
                node->next = temp;
                return;
            }
        }
        prev   = temp;
        temp   = temp->next;
        isHead = false;
    }

    prev->next = node;
    node->next = nullptr;
}

void LinkedList::remove(NavNode* node) {
    if (head == nullptr) return;

    if (head == node) {
        head = head->next;
        return;
    }

    NavNode* temp = head;
    while (temp->next != nullptr) {
        if (temp->next == node) {
            temp->next = node->next;
            return;
        }
        temp = temp->next;
    }
}


// PriorityQueue
void PriorityQueue::insert(int nodeID, int distance) {
    list.minInsert(new NavNode(nodeID, distance));
}

void PriorityQueue::insert(NavNode* node) {
    list.minInsert(node);
}

NavNode* PriorityQueue::extractMin() {
    return list.removeHead();
}

bool PriorityQueue::isEmpty() {
    return list.head == nullptr;
}

void PriorityQueue::decreaseKey(int nodeID, int newDistance) {
    NavNode* curr = list.head;
    while (curr != nullptr) {
        if (curr->nodeID == nodeID) {
            curr->distance = newDistance;
            list.remove(curr);
            list.minInsert(curr);
            return;
        }
        curr = curr->next;
    }

    list.minInsert(new NavNode(nodeID, newDistance));
}


// Stack
void Stack::push(int id, const std::string& direction) {
    list.insertHead(new NavNode(id, direction));
}

NavNode* Stack::pop() {
    return list.removeHead();
}

bool Stack::isEmpty() {
    return list.head == nullptr;
}

LinkedList& Stack::getList() {
    return list;
}


// Graph
Graph::Graph(int nodeCount) : nodeCount(nodeCount) {}

void Graph::addEdge(int src, int dest, int dist) {
    adjList[src].minInsert(new NavNode(dest, dist));
    adjList[dest].minInsert(new NavNode(src, dist));
}

NavNode* Graph::getNeighbours(int nodeID) {
    return adjList[nodeID].head;
}

int Graph::getNodeCount() const {
    return nodeCount;
}


// Warehouse - hardcoded 12-node layout
Warehouse::Warehouse() : graph(12), nodeCount(12) {
    x[0]=5;   y[0]=5;
    x[1]=15;  y[1]=5;
    x[2]=30;  y[2]=5;
    x[3]=45;  y[3]=5;
    x[4]=5;   y[4]=20;
    x[5]=20;  y[5]=20;
    x[6]=35;  y[6]=20;
    x[7]=45;  y[7]=20;
    x[8]=5;   y[8]=40;
    x[9]=15;  y[9]=40;
    x[10]=30; y[10]=40;
    x[11]=45; y[11]=40;

    graph.addEdge(0,1,10);  graph.addEdge(0,4,15);  graph.addEdge(0,5,18);
    graph.addEdge(1,2,15);  graph.addEdge(1,4,18);  graph.addEdge(1,5,12);
    graph.addEdge(2,3,15);  graph.addEdge(2,5,14);  graph.addEdge(2,6,15);
    graph.addEdge(3,6,18);  graph.addEdge(3,7,15);
    graph.addEdge(4,5,15);  graph.addEdge(4,8,20);  graph.addEdge(4,9,18);
    graph.addEdge(5,6,15);  graph.addEdge(5,8,22);  graph.addEdge(5,9,20);
    graph.addEdge(5,10,18);
    graph.addEdge(6,7,15);  graph.addEdge(6,9,22);  graph.addEdge(6,10,15);
    graph.addEdge(6,11,18);
    graph.addEdge(7,10,18); graph.addEdge(7,11,25);
    graph.addEdge(8,9,10);
    graph.addEdge(9,10,15);
    graph.addEdge(10,11,15);
}

NavNode* Warehouse::getNeighbours(int nodeID) { return graph.getNeighbours(nodeID); }
int      Warehouse::getNodeCount()      const { return nodeCount; }
int      Warehouse::getX(int nodeID)    const { return x[nodeID]; }
int      Warehouse::getY(int nodeID)    const { return y[nodeID]; }

// Convert two adjacent node positions into a human-readable
std::string Warehouse::getDirection(int from, int to) const {
    int dx = x[to] - x[from];
    int dy = y[to] - y[from];

    int nx = (dx > 0) - (dx < 0);   
    int ny = (dy > 0) - (dy < 0);

    if (nx==1  && ny==0)  return "Right";
    if (nx==-1 && ny==0)  return "Left";
    if (nx==0  && ny==1)  return "Down";
    if (nx==0  && ny==-1) return "Up";
    if (nx==1  && ny==1)  return "Down-Right";
    if (nx==-1 && ny==1)  return "Down-Left";
    if (nx==1  && ny==-1) return "Up-Right";
    if (nx==-1 && ny==-1) return "Up-Left";
    return "None";
}


// Robot
Robot::Robot(Warehouse& w, int startPosition, const std::string& robotId)
    : warehouse(w), position(startPosition), isFree(true), id(robotId) {}

// Two-step navigation: dispatchForward + returnHome.
void Robot::dispatchForward(int target) {
    while (!returnStack.isEmpty()) {
        NavNode* leftover = returnStack.pop();
        delete leftover;
    }

    Stack forward;

    if (memo[target].head != nullptr) {
        std::cout << "  [Module 3] Cached path to node " << target
                  << " found, reusing\n";
        NavNode* curr = memo[target].head;
        while (curr != nullptr) {
            forward.push(curr->nodeID, curr->direction);
            curr = curr->next;
        }
    } else {
        std::cout << "  [Module 3] No cached path, running Dijkstra\n";
        pathfinder(target, forward);
    }

    std::cout << "  Forward path:\n";
    while (!forward.isEmpty()) {
        NavNode* step = forward.pop();
        std::cout << "    Node " << step->nodeID
                  << " | " << step->direction << "\n";
        returnStack.push(step->nodeID, step->direction);
        delete step;
    }

    position = target;
}

void Robot::returnHome() {
    if (returnStack.isEmpty()) {
        std::cout << "  No return path stored (robot already home).\n";
        position = 0;
        return;
    }
    std::cout << "  Return path (popping stack):\n";
    while (!returnStack.isEmpty()) {
        NavNode* step = returnStack.pop();
        std::cout << "    Node " << step->nodeID
                  << " | " << step->direction << "\n";
        delete step;
    }
    position = 0;
}


// One-shot navigation 
void Robot::assignTarget(int target, bool shouldReverse) {

    Stack forward;

    if (memo[target].head != nullptr) {
        std::cout << "  [Module 3] Cached path to node " << target
                  << " found, reusing\n";
        NavNode* curr = memo[target].head;
        while (curr != nullptr) {
            forward.push(curr->nodeID, curr->direction);
            curr = curr->next;
        }
    } else {
        std::cout << "  [Module 3] No cached path, running Dijkstra\n";
        pathfinder(target, forward);
    }

    Stack reverse;
    std::cout << "  -- Forward path --\n";
    while (!forward.isEmpty()) {
        NavNode* step = forward.pop();
        std::cout << "    Node " << step->nodeID
                  << " | " << step->direction << "\n";
        reverse.push(step->nodeID, step->direction);
        delete step; 
    }

    if (shouldReverse) {
        std::cout << "  -- Return path --\n";
        while (!reverse.isEmpty()) {
            NavNode* step = reverse.pop();
            std::cout << "    Node " << step->nodeID
                      << " | " << step->direction << "\n";
            delete step;
        }
        position = 0; 
    }
}

// Dijkstra's algorithm
void Robot::pathfinder(int target, Stack& out) {
    const int n = warehouse.getNodeCount();

    int dist[MAX_NODES];
    int prev[MAX_NODES];
    for (int i = 0; i < n; i++) {
        dist[i] = INT_MAX;
        prev[i] = -1;
    }

    dist[position] = 0;
    PriorityQueue pq;
    pq.insert(new NavNode(position, 0));

    while (!pq.isEmpty()) {
        NavNode* temp = pq.extractMin();
        int u = temp->nodeID;
        int tempDist = temp->distance;
        delete temp;

        if (tempDist > dist[u]) continue;

        NavNode* current = warehouse.getNeighbours(u);
        while (current != nullptr) {
            int v = current->nodeID;
            int w = current->distance;

            if (dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.decreaseKey(v, dist[v]);
            }
            current = current->next;
        }
    }

    if (dist[target] == INT_MAX) {
        std::cout << "  [Module 3] No path found to node " << target << "\n";
        return;
    }


    out.push(target, "Arrived");
    for (int i = target; i != -1; i = prev[i]) {
        if (prev[i] == -1) break;
        std::string dir = warehouse.getDirection(prev[i], i);
        out.push(prev[i], dir);
    }

    int path[MAX_NODES];
    int count = 0;
    for (int i = target; i != -1; i = prev[i]) {
        if (prev[i] == -1) break;
        path[count++] = i;
    }
    for (int i = count - 1; i >= 0; i--) {
        std::string dir = warehouse.getDirection(prev[path[i]], path[i]);
        memo[target].insertHead(new NavNode(prev[path[i]], dir));
    }
    memo[target].insertHead(new NavNode(target, "Arrived"));

    position = target;
}

int                Robot::getPosition()     const { return position; }
bool               Robot::getAvailability() const { return isFree; }
const std::string& Robot::getId()           const { return id; }

void Robot::releaseAvailability() { isFree = true;  }
void Robot::claimAvailability()   { isFree = false; }
