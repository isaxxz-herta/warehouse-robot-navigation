// Module3.hpp
// Delfino - TP078795 (Robot Navigation and Path Tracking)

#ifndef MODULE3_HPP
#define MODULE3_HPP

#include "Common.hpp"
#include <string>

// NavNode 
struct NavNode {
    std::string direction;
    int         nodeID;
    int         distance;
    NavNode*    next;

    NavNode(int id, const std::string& dir);   
    NavNode(int nodeID, int distance);         
};


// LinkedList 
class LinkedList {
public:
    NavNode* head;

    LinkedList();
    ~LinkedList();

    LinkedList(const LinkedList&)            = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void     insertHead(NavNode* node);    
    NavNode* removeHead();                 
    void     minInsert(NavNode* node);     
    void     remove(NavNode* node);       
    int      getLength() const;
    void     printList() const;
};


// PriorityQueue 
class PriorityQueue {
    LinkedList list;
public:
    void     insert(int nodeID, int distance);
    void     insert(NavNode* node);
    NavNode* extractMin();
    bool     isEmpty();
    void     decreaseKey(int nodeID, int newDistance);
};


// Stack - LIFO
class Stack {
    LinkedList list;
public:
    void     push(int id, const std::string& direction);
    NavNode* pop();
    bool     isEmpty();
    LinkedList& getList();
};


// Graph 
const int MAX_NODES = 30;

class Graph {
    LinkedList adjList[MAX_NODES]; 
    int        nodeCount;
public:
    explicit Graph(int nodeCount);
    void     addEdge(int src, int dest, int dist);
    NavNode* getNeighbours(int nodeID);
    int      getNodeCount() const;
};


// Warehouse 
class Warehouse {
    Graph graph;
    int   x[12];
    int   y[12];
    int   nodeCount;
public:
    Warehouse();

    NavNode*    getNeighbours(int nodeID);
    int         getNodeCount() const;
    int         getX(int nodeID) const;
    int         getY(int nodeID) const;
    std::string getDirection(int from, int to) const;
};


// Robot position, availability, pathfinder. 
class Robot {
private:
    Warehouse&  warehouse;
    int         position;
    bool        isFree;
    std::string id;         

    LinkedList memo[12];

    Stack returnStack;

    void pathfinder(int target, Stack& out);

public:
    Robot(Warehouse& w, int startPosition, const std::string& id = "");

    void dispatchForward(int target);

    void returnHome();

    void assignTarget(int target, bool shouldReturn);

    int         getPosition()      const;
    bool        getAvailability()  const;
    const std::string& getId()     const;

    void releaseAvailability();

    void claimAvailability();
};

#endif 
