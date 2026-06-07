// Module1.cpp (Order Management)
// Roger Rusdijanto Purnomo - TP075718

#include "Module1.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>


// OrderQueue
OrderQueue::OrderQueue(int capacity)
    : head(nullptr), tail(nullptr), count(0), maxSize(capacity) {}

OrderQueue::~OrderQueue() {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
    count = 0;
}

bool OrderQueue::enqueue(const Order& o) {
    if (isFull()) {
        return false;
    }

    Node* newNode = new Node(o);

    if (head == nullptr) {
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail       = newNode;
    }
    count++;
    return true;
}

bool OrderQueue::dequeue(Order& outOrder) {
    if (isEmpty()) {
        return false;
    }

    Node* front = head;
    outOrder    = front->data;
    head        = front->next;
    if (head == nullptr) {
        tail = nullptr; 
    }
    delete front;
    count--;
    return true;
}

bool OrderQueue::peek(Order& outOrder) const {
    if (isEmpty()) return false;
    outOrder = head->data;
    return true;
}

bool OrderQueue::isEmpty() const { return head == nullptr; }

bool OrderQueue::isFull() const {
    if (maxSize < 0) return false; 
    return count >= maxSize;
}

int OrderQueue::size() const { return count; }

int OrderQueue::capacity() const { return maxSize; }

void OrderQueue::setMaxSize(int newMax) {
    maxSize = newMax;
}

void OrderQueue::displayAll() const {
    if (isEmpty()) {
        std::cout << "  (no pending orders)\n";
        return;
    }

    int   index   = 1;
    Node* current = head;
    while (current != nullptr) {
        const Order& o = current->data;
        std::cout << "  " << index << ". " << o.orderId
                  << " | " << o.customerName
                  << " | item=" << o.itemId
                  << " x" << o.quantity
                  << " | arrived " << o.timestamp
                  << " | " << statusToString(o.status) << "\n";
        current = current->next;
        index++;
    }
}


// OrderHistory
OrderHistory::OrderHistory()
    : head(nullptr), tail(nullptr), count(0) {}

OrderHistory::~OrderHistory() {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head = tail = nullptr;
    count = 0;
}

void OrderHistory::append(const Order& o) {
    Order completed = o;
    completed.status = OrderStatus::COMPLETED;

    Node* newNode = new Node(completed);

    if (tail == nullptr) {
        head = tail = newNode;
    } else {
        newNode->prev = tail;
        tail->next    = newNode;
        tail          = newNode;
    }
    count++;
}

int  OrderHistory::size()    const { return count; }
bool OrderHistory::isEmpty() const { return head == nullptr; }

void OrderHistory::displayNewestFirst() const {
    if (isEmpty()) {
        std::cout << "  (no completed orders yet)\n";
        return;
    }
    int   index   = 1;
    Node* current = tail;
    while (current != nullptr) {
        const Order& o = current->data;
        std::cout << "  " << index << ". " << o.orderId
                  << " | " << o.customerName
                  << " | item=" << o.itemId
                  << " x" << o.quantity
                  << " | arrived " << o.timestamp << "\n";
        current = current->prev;
        index++;
    }
}

void OrderHistory::displayOldestFirst() const {
    if (isEmpty()) {
        std::cout << "  (no completed orders yet)\n";
        return;
    }
    int   index   = 1;
    Node* current = head;
    while (current != nullptr) {
        const Order& o = current->data;
        std::cout << "  " << index << ". " << o.orderId
                  << " | " << o.customerName
                  << " | item=" << o.itemId
                  << " x" << o.quantity
                  << " | arrived " << o.timestamp << "\n";
        current = current->next;
        index++;
    }
}


// OrderManager
OrderManager::OrderManager()
    : pending(),
      history(),
      current(),
      hasCurrent(false),
      currentRobotId(""),
      nextAutoId(1) {}

OrderStatus OrderManager::parseStatus(const std::string& s) {
    if (s == "completed")  return OrderStatus::COMPLETED;
    if (s == "processing") return OrderStatus::PROCESSING;
    return OrderStatus::PENDING;
}


bool OrderManager::parseCsvLine(const std::string& line, Order& out) const {
    const int EXPECTED_COLS = 22;
    std::string fields[EXPECTED_COLS];
    int fieldIndex = 0;

    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        if (fieldIndex >= EXPECTED_COLS) return false;
        fields[fieldIndex++] = token;
    }
    if (fieldIndex != EXPECTED_COLS) return false;

    int qty = 0;
    try {
        qty = std::stoi(fields[20]);
    } catch (...) {
        return false;
    }
    if (qty <= 0) return false;

    out.orderId      = fields[0];
    out.customerName = fields[1];
    out.timestamp    = fields[2];
    out.status       = parseStatus(fields[3]);
    out.itemId       = fields[14];
    out.quantity     = qty;
    return true;
}

int OrderManager::loadFromCSV(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "  [warn] could not open " << path
                  << " - starting with empty system.\n";
        return 0;
    }

    std::string line;
    bool firstLine = true;
    int  loaded    = 0;

    std::string lastOrderId = "";

    while (std::getline(file, line)) {
        if (firstLine) { firstLine = false; continue; }
        if (line.empty()) continue;

        Order o;
        if (!parseCsvLine(line, o)) {
            std::cout << "  [warn] skipped malformed row: " << line << "\n";
            continue;
        }
        if (o.orderId == lastOrderId) continue;
        lastOrderId = o.orderId;

        switch (o.status) {
            case OrderStatus::COMPLETED:
                history.append(o);
                loaded++;
                break;

            case OrderStatus::PROCESSING:
                o.status = OrderStatus::PENDING;
                if (pending.enqueue(o)) loaded++;
                break;

            case OrderStatus::PENDING:
            default:
                if (pending.enqueue(o)) loaded++;
                break;
        }
    }

    file.close();
    return loaded;
}

bool OrderManager::addOrder(const Order& o) {
    if (!pending.enqueue(o)) {
        std::cout << "  [error] system overload: pending queue is full. "
                  << "Order rejected.\n";
        return false;
    }
    return true;
}

bool OrderManager::addOrder(const std::string& customerName,
                            const std::string& itemId,
                            int quantity,
                            const std::string& timestamp) {
    if (quantity <= 0) {
        std::cout << "  [error] quantity must be a positive integer.\n";
        return false;
    }

    std::ostringstream idStream;
    idStream << "ORD" << std::setw(3) << std::setfill('0')
             << (100 + nextAutoId);
    nextAutoId++;

    Order o(idStream.str(), customerName, itemId,
            quantity, timestamp, OrderStatus::PENDING);
    return addOrder(o);
}

bool OrderManager::assignNextToRobot(Order& outOrder,
                                     const std::string& robotId) {
    if (hasCurrent) {
        std::cout << "  [info] a current order is already being processed ("
                  << current.orderId << " by " << currentRobotId
                  << "). Mark it completed before assigning the next one.\n";
        return false;
    }
    if (!pending.dequeue(outOrder)) {
        std::cout << "  [info] no pending orders to assign.\n";
        return false;
    }

    outOrder.status = OrderStatus::PROCESSING;
    current         = outOrder;
    hasCurrent      = true;
    currentRobotId  = robotId;
    return true;
}

bool OrderManager::markCurrentCompleted() {
    if (!hasCurrent) {
        std::cout << "  [info] no order is currently being processed.\n";
        return false;
    }
    current.status = OrderStatus::COMPLETED;
    history.append(current);

    hasCurrent     = false;
    currentRobotId = "";
    current        = Order();
    return true;
}

bool OrderManager::getCurrentOrder(Order& out, std::string& robotOut) const {
    if (!hasCurrent) return false;
    out      = current;
    robotOut = currentRobotId;
    return true;
}

int  OrderManager::pendingCount()   const { return pending.size(); }
int  OrderManager::completedCount() const { return history.size(); }
bool OrderManager::hasPending()     const { return !pending.isEmpty(); }
bool OrderManager::isProcessing()   const { return hasCurrent; }

void OrderManager::setQueueCapacity(int newCap) { pending.setMaxSize(newCap); }
int  OrderManager::getQueueCapacity()     const { return pending.capacity(); }

void OrderManager::displayPending() const {
    std::cout << "Pending Orders (" << pending.size() << ")\n";
    pending.displayAll();
}

void OrderManager::displayCurrent() const {
    std::cout << "Current Order Being Processed\n";
    if (!hasCurrent) {
        std::cout << "  (idle: no order is being processed right now)\n";
        return;
    }
    std::cout << "  " << current.orderId
              << " | " << current.customerName
              << " | item=" << current.itemId
              << " x" << current.quantity
              << " | assigned to robot " << currentRobotId << "\n";
}

void OrderManager::displayHistoryNewestFirst() const {
    std::cout << "Completed Orders (newest first, "
              << history.size() << " total)\n";
    history.displayNewestFirst();
}

void OrderManager::displayHistoryOldestFirst() const {
    std::cout << "Completed Orders (oldest first, "
              << history.size() << " total)\n";
    history.displayOldestFirst();
}

void OrderManager::displaySummary() const {
    int cap = pending.capacity();
    std::cout << "Module 1 Summary\n"
              << "  Pending  : " << pending.size()
              << " (capacity: "
              << (cap < 0 ? std::string("unlimited") : std::to_string(cap))
              << ")\n"
              << "  Current  : " << (hasCurrent ? current.orderId : "(none)")
              << (hasCurrent ? (" -> " + currentRobotId) : "") << "\n"
              << "  Completed: " << history.size() << "\n";
}
