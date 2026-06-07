// Module2.hpp
// Isa Ng Bin Mohd Jaafar Ng - TP086267

#ifndef MODULE2_HPP
#define MODULE2_HPP

#include "Common.hpp"
#include "Module3.hpp"   
#include <string>

// QueueNode - one slot in the circular queue. Holds a Robot pointer
struct QueueNode {
    Robot*      robotPtr;
    std::string robotID;
    std::string currentOrderID; 
    int         targetNode;    
    QueueNode*  next;

    QueueNode(Robot* robotPtr, const std::string& robotID)
        : robotPtr(robotPtr), robotID(robotID),
          currentOrderID(""), targetNode(-1), next(nullptr) {}
};


// CircularQueue - circular linked list with a `rear` pointer.
class CircularQueue {
private:
    QueueNode* rear;

public:
    CircularQueue();
    ~CircularQueue();

    CircularQueue(const CircularQueue&)            = delete;
    CircularQueue& operator=(const CircularQueue&) = delete;

    bool       isEmpty() const;
    void       enqueue(Robot* robotPtr, const std::string& robotID);

    QueueNode* getNextAvailableRobot();

    QueueNode* findByID(const std::string& robotID) const;

    void displayRobots() const;
};


// AssignmentNode / AssignmentHistory - simple singly linked list
struct AssignmentNode {
    std::string     orderID;
    std::string     robotID;
    AssignmentNode* next;

    AssignmentNode(const std::string& orderID, const std::string& robotID)
        : orderID(orderID), robotID(robotID), next(nullptr) {}
};

class AssignmentHistory {
private:
    AssignmentNode* head;

public:
    AssignmentHistory();
    ~AssignmentHistory();

    AssignmentHistory(const AssignmentHistory&)            = delete;
    AssignmentHistory& operator=(const AssignmentHistory&) = delete;

    bool isEmpty() const;
    void addRecord(const std::string& orderID, const std::string& robotID);
    void displayHistory() const;
};


// RobotAssignment - facade exposed to main.cpp. Owns the circular
class RobotAssignment {
private:
    CircularQueue     robots;
    AssignmentHistory history;

public:
    void addRobot(Robot* robotPtr, const std::string& robotID);

    Robot* assignNextRobot(const Order& order, std::string& robotIDOut);

    void markRobotAvailable(const std::string& robotID);

    bool recordManualAssignment(const std::string& orderID,
                                const std::string& robotID,
                                int targetNode);
    Robot* findRobotById(const std::string& robotID) const;

    void displayRobots()            const;
    void displayAssignmentHistory() const;

    int loadRobotsFromCSV(const std::string& path, Warehouse& warehouse);

    int  robotCount() const;

    RobotAssignment();
    ~RobotAssignment();

private:
    static const int MAX_ROBOTS = 16;
    Robot* robotPool[MAX_ROBOTS];
    int    robotPoolCount;
};

#endif 
