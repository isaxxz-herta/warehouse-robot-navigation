// Common.hpp
// Shared types and small helpers used by more than one module.


#ifndef COMMON_HPP
#define COMMON_HPP

#include <string>

// Lifecycle of an order inside the warehouse system.
enum class OrderStatus {
    PENDING,
    PROCESSING,
    COMPLETED
};


struct Order {
    std::string orderId;       
    std::string customerName;  
    std::string itemId;        
    int         quantity;     
    std::string timestamp;     
    OrderStatus status;        

    Order()
        : orderId(""), customerName(""), itemId(""),
          quantity(0), timestamp(""), status(OrderStatus::PENDING) {}

    Order(const std::string& id,
          const std::string& customer,
          const std::string& item,
          int qty,
          const std::string& ts,
          OrderStatus st)
        : orderId(id), customerName(customer), itemId(item),
          quantity(qty), timestamp(ts), status(st) {}
};

inline std::string statusToString(OrderStatus s) {
    switch (s) {
        case OrderStatus::PENDING:    return "pending";
        case OrderStatus::PROCESSING: return "processing";
        case OrderStatus::COMPLETED:  return "completed";
    }
    return "unknown";
}

#endif 
