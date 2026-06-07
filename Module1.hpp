// Module1.hpp - OrderManager, OrderQueue, and OrderHistory class declarations
// Roger Rusdijanto Purnomo - TP075718

#ifndef MODULE1_HPP
#define MODULE1_HPP

#include "Common.hpp"
#include <string>

// OrderQueue - self-made singly linked list FIFO queue
class OrderQueue {
private:
    struct Node {
        Order data;
        Node* next;
        Node(const Order& o) : data(o), next(nullptr) {}
    };

    Node* head;   
    Node* tail;    
    int   count;   
    int   maxSize; 

public:
    explicit OrderQueue(int capacity = -1);
    ~OrderQueue();

    OrderQueue(const OrderQueue&)            = delete;
    OrderQueue& operator=(const OrderQueue&) = delete;

    bool enqueue(const Order& o);    
    bool dequeue(Order& outOrder);   
    bool peek(Order& outOrder) const;
    bool isEmpty() const;
    bool isFull()  const;           
    int  size()    const;
    int  capacity() const;           
    void setMaxSize(int newMax);     

    void displayAll() const;
};


// OrderHistory - self-made doubly linked list
class OrderHistory {
private:
    struct Node {
        Order data;
        Node* prev;
        Node* next;
        Node(const Order& o) : data(o), prev(nullptr), next(nullptr) {}
    };

    Node* head;   
    Node* tail;   
    int   count;

public:
    OrderHistory();
    ~OrderHistory();

    OrderHistory(const OrderHistory&)            = delete;
    OrderHistory& operator=(const OrderHistory&) = delete;

    void append(const Order& o);    
    int  size()    const;
    bool isEmpty() const;

    void displayNewestFirst() const; 
    void displayOldestFirst() const; 
};


// OrderManager 
class OrderManager {
private:
    OrderQueue   pending;
    OrderHistory history;

    Order       current;
    bool        hasCurrent;
    std::string currentRobotId;

    int nextAutoId;

    bool parseCsvLine(const std::string& line, Order& out) const;
    static OrderStatus parseStatus(const std::string& s);

public:
    OrderManager();
    int  loadFromCSV(const std::string& path);

    bool addOrder(const std::string& customerName,
                  const std::string& itemId,
                  int quantity,
                  const std::string& timestamp);
    bool addOrder(const Order& o);

    bool assignNextToRobot(Order& outOrder, const std::string& robotId);

    bool markCurrentCompleted();

    bool getCurrentOrder(Order& out, std::string& robotOut) const;
    int  pendingCount()   const;
    int  completedCount() const;
    bool hasPending()     const;
    bool isProcessing()   const;

    void setQueueCapacity(int newCap);
    int  getQueueCapacity() const;

    void displayPending()             const;
    void displayCurrent()             const;
    void displayHistoryNewestFirst()  const;
    void displayHistoryOldestFirst()  const;
    void displaySummary()             const;
};

#endif
