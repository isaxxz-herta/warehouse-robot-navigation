// Module2.cpp (Robot Assignment)
// Isa Ng Bin Mohd Jaafar Ng - TP086267

#include "Module2.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

// CircularQueue
CircularQueue::CircularQueue() : rear(nullptr) {}

CircularQueue::~CircularQueue() {
    if (rear == nullptr) return;

    QueueNode* head = rear->next;
    rear->next      = nullptr;

    while (head != nullptr) {
        QueueNode* next = head->next;
        delete head;
        head = next;
    }
    rear = nullptr;
}

bool CircularQueue::isEmpty() const {
    return rear == nullptr;
}

void CircularQueue::enqueue(Robot* robotPtr, const std::string& robotID) {
    QueueNode* newNode = new QueueNode(robotPtr, robotID);

    if (rear == nullptr) {
        rear        = newNode;
        rear->next  = rear;
        return;
    }

    newNode->next = rear->next;
    rear->next    = newNode;
    rear          = newNode;
}

QueueNode* CircularQueue::getNextAvailableRobot() {
    if (rear == nullptr) return nullptr;

    QueueNode* current = rear->next;

    do {
        if (current->robotPtr->getAvailability()) {
            rear = current;
            return current;
        }
        current = current->next;
    } while (current != rear->next);

    return nullptr;
}

QueueNode* CircularQueue::findByID(const std::string& robotID) const {
    if (rear == nullptr) return nullptr;

    QueueNode* current = rear->next;
    do {
        if (current->robotID == robotID) return current;
        current = current->next;
    } while (current != rear->next);

    return nullptr;
}

void CircularQueue::displayRobots() const {
    if (rear == nullptr) {
        std::cout << "  (no robots loaded)\n";
        return;
    }

    QueueNode* current = rear->next;
    do {
        Robot* r = current->robotPtr;
        std::cout << "  " << current->robotID
                  << " | position node=" << r->getPosition()
                  << " | "
                  << (r->getAvailability() ? "available" : "busy");
        if (!current->currentOrderID.empty()) {
            std::cout << " | working on " << current->currentOrderID;
            if (current->targetNode >= 0) {
                std::cout << " (target node " << current->targetNode << ")";
            }
        }
        std::cout << "\n";
        current = current->next;
    } while (current != rear->next);
}

// AssignmentHistory
AssignmentHistory::AssignmentHistory() : head(nullptr) {}

AssignmentHistory::~AssignmentHistory() {
    AssignmentNode* current = head;
    while (current != nullptr) {
        AssignmentNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
}

bool AssignmentHistory::isEmpty() const {
    return head == nullptr;
}

void AssignmentHistory::addRecord(const std::string& orderID,
                                  const std::string& robotID) {
    AssignmentNode* record = new AssignmentNode(orderID, robotID);

    if (head == nullptr) {
        head = record;
        return;
    }

    AssignmentNode* current = head;
    while (current->next != nullptr) current = current->next;
    current->next = record;
}

void AssignmentHistory::displayHistory() const {
    if (head == nullptr) {
        std::cout << "  (no assignments yet)\n";
        return;
    }

    AssignmentNode* current = head;
    int             index   = 1;
    while (current != nullptr) {
        std::cout << "  " << index << ". "
                  << current->orderID << " -> "
                  << current->robotID << "\n";
        current = current->next;
        index++;
    }
}


// RobotAssignment
RobotAssignment::RobotAssignment() : robotPoolCount(0) {
    for (int i = 0; i < MAX_ROBOTS; i++) robotPool[i] = nullptr;
}

RobotAssignment::~RobotAssignment() {
    for (int i = 0; i < robotPoolCount; i++) {
        delete robotPool[i];
        robotPool[i] = nullptr;
    }
    robotPoolCount = 0;
}

void RobotAssignment::addRobot(Robot* robotPtr, const std::string& robotID) {
    robots.enqueue(robotPtr, robotID);
}

Robot* RobotAssignment::assignNextRobot(const Order& order,
                                       std::string& robotIDOut) {
    QueueNode* picked = robots.getNextAvailableRobot();
    if (picked == nullptr) {
        std::cout << "  [Module 2] No available robot for "
                  << order.orderId << " - all robots are busy.\n";
        return nullptr;
    }

    picked->robotPtr->claimAvailability();
    picked->currentOrderID = order.orderId;

    history.addRecord(order.orderId, picked->robotID);
    robotIDOut = picked->robotID;
    return picked->robotPtr;
}

void RobotAssignment::markRobotAvailable(const std::string& robotID) {
    QueueNode* node = robots.findByID(robotID);
    if (node == nullptr) {
        std::cout << "  [Module 2] markRobotAvailable: unknown robot "
                  << robotID << "\n";
        return;
    }
    node->robotPtr->releaseAvailability();
    node->currentOrderID = ""; 
    node->targetNode     = -1;
}


bool RobotAssignment::recordManualAssignment(const std::string& orderID,
                                             const std::string& robotID,
                                             int targetNode) {
    QueueNode* node = robots.findByID(robotID);
    if (node == nullptr) {
        std::cout << "  [Module 2] Unknown robot " << robotID
                  << " - assignment not recorded.\n";
        return false;
    }
    if (!node->robotPtr->getAvailability()) {
        std::cout << "  [Module 2] Robot " << robotID
                  << " is already busy with "
                  << (node->currentOrderID.empty()
                      ? std::string("another task")
                      : node->currentOrderID)
                  << ".\n";
        return false;
    }
    node->robotPtr->claimAvailability();
    node->currentOrderID = orderID;
    node->targetNode     = targetNode;
    history.addRecord(orderID, robotID);
    return true;
}

void RobotAssignment::displayRobots() const {
    std::cout << "Robot Roster\n";
    robots.displayRobots();
}

void RobotAssignment::displayAssignmentHistory() const {
    std::cout << "Assignment History\n";
    history.displayHistory();
}

int RobotAssignment::robotCount() const {
    return robotPoolCount;
}

Robot* RobotAssignment::findRobotById(const std::string& robotID) const {
    QueueNode* node = robots.findByID(robotID);
    if (node == nullptr) return nullptr;
    return node->robotPtr;
}


int RobotAssignment::loadRobotsFromCSV(const std::string& path,
                                      Warehouse& warehouse) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "  [Module 2] could not open " << path << "\n";
        return 0;
    }

    std::string line;
    bool        firstLine = true;
    int         loaded    = 0;

    std::string seen[MAX_ROBOTS];
    int         seenCount = 0;

    while (std::getline(file, line)) {
        if (firstLine) { firstLine = false; continue; }
        if (line.empty()) continue;

        std::string fields[22];
        int         fieldIndex = 0;
        std::stringstream ss(line);
        std::string       token;
        while (std::getline(ss, token, ',')) {
            if (fieldIndex >= 22) break;
            fields[fieldIndex++] = token;
        }
        if (fieldIndex < 9) continue; 

        std::string robotID     = fields[4];
        std::string robotStatus = fields[6]; 
        if (robotID.empty()) continue;

        bool already = false;
        for (int i = 0; i < seenCount; i++) {
            if (seen[i] == robotID) { already = true; break; }
        }
        if (already) continue;
        if (seenCount >= MAX_ROBOTS) break; 

        Robot* r = new Robot(warehouse, 0, robotID);

        if (robotStatus == "maintenance" || robotStatus == "busy") {
            r->claimAvailability();
        }

        robotPool[robotPoolCount++] = r;
        seen[seenCount++]           = robotID;

        addRobot(r, robotID);
        loaded++;
    }

    file.close();
    return loaded;
}
