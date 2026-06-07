// main.cpp
// Module 1 - Roger - TP075718(Order Management)
// Module 2 - Isa - TP086267 (Robot Assignment)
// Module 3 - Delfino - TP078795 (Robot Navigation and Path Tracking)
// Module 4 - Ryan - TP082108 (Item Search and Management)

#include "Common.hpp"
#include "Module1.hpp"
#include "Module2.hpp"
#include "Module3.hpp"
#include "Module4.hpp"

#include <iostream>
#include <string>
#include <ctime>

static const std::string DATA_FILE = "warehouse_data.csv";

// Input helpers
static std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string line;
    std::getline(std::cin, line);
    return line;
}

static int readInt(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) return 0; // EOF
        try { return std::stoi(line); }
        catch (...) { std::cout << "  Please enter a whole number.\n"; }
    }
}

static std::string currentTimestampString() {
    std::time_t now = std::time(nullptr);
    std::tm tmStruct{};
#if defined(_WIN32)
    localtime_s(&tmStruct, &now);
#else
    localtime_r(&now, &tmStruct);
#endif
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tmStruct);
    return std::string(buffer);
}

// Module 1 (Order Management) sub menu
static void menuModule1(OrderManager&    om,
                        RobotAssignment& scheduler,
                        ItemBST&         items) {
    bool back = false;
    while (!back) {
        std::cout << "\nModule 1: Order Management\n"
                  << "1. View pending orders\n"
                  << "2. View current order being processed\n"
                  << "3. View completed history (newest first)\n"
                  << "4. View completed history (oldest first)\n"
                  << "5. Add a new customer order\n"
                  << "6. Assign next pending order to a robot\n"
                  << "7. Mark current order as completed\n"
                  << "8. Set pending queue capacity (overload demo)\n"
                  << "9. Show summary\n"
                  << "0. Back to main menu\n";
        int choice = readInt("Select: ");
        switch (choice) {
            case 1: om.displayPending();             break;
            case 2: om.displayCurrent();             break;
            case 3: om.displayHistoryNewestFirst();  break;
            case 4: om.displayHistoryOldestFirst();  break;
            case 5: {
                std::string customer = readLine("  Customer name : ");
                std::string item     = readLine("  Item ID       : ");
                int         qty      = readInt ("  Quantity      : ");
                if (om.addOrder(customer, item, qty, currentTimestampString())) {
                    std::cout << "  Order accepted into the pending queue.\n";
                }
                break;
            }
            case 6: {
                std::string robotId = readLine("  Robot ID to assign to (e.g. R01): ");

                Order peek;
                if (!om.assignNextToRobot(peek, robotId)) break;

                int targetNode = items.findLocationNodeByString(peek.itemId);
                if (targetNode < 0) {
                    std::cout << "  Item " << peek.itemId
                              << " not found in Module 4. Defaulting to node 0.\n";
                    targetNode = 0;
                }

                if (!scheduler.recordManualAssignment(peek.orderId,
                                                     robotId,
                                                     targetNode)) {
                    std::cout << "  Order " << peek.orderId
                              << " could not be assigned - rolling back.\n";
                    om.markCurrentCompleted();
                    break;
                }

                Robot* picked = nullptr;
                {
                    Order tmp;
                    std::string ignore;
                    (void)tmp; (void)ignore;
                }

                picked = scheduler.findRobotById(robotId);
                if (picked != nullptr) {
                    std::cout << "  Module 3 dispatching " << robotId
                              << " to node " << targetNode << ":\n";
                    picked->dispatchForward(targetNode);
                }

                std::cout << "  Order " << peek.orderId
                          << " is now in flight: robot " << robotId
                          << " at node " << targetNode << ".\n";
                break;
            }
            case 7: {
                Order       cur;
                std::string busyRobot;
                bool        hadCurrent = om.getCurrentOrder(cur, busyRobot);
                if (!hadCurrent) {
                    std::cout << "  No order is currently being processed.\n";
                    break;
                }

                Robot* r = scheduler.findRobotById(busyRobot);
                if (r != nullptr) {
                    std::cout << "  Module 3 returning " << busyRobot
                              << " home via stack pop:\n";
                    r->returnHome();
                }

                if (!om.markCurrentCompleted()) break;
                scheduler.markRobotAvailable(busyRobot);
                std::cout << "  Order " << cur.orderId
                          << " marked completed; robot " << busyRobot
                          << " is now available.\n";
                break;
            }
            case 8: {
                std::cout << "  Current capacity: ";
                int cur = om.getQueueCapacity();
                if (cur < 0) std::cout << "unlimited\n";
                else         std::cout << cur << "\n";
                int newCap = readInt("  New capacity (-1 for unlimited): ");
                om.setQueueCapacity(newCap);
                std::cout << "  Capacity updated. Try option 5 to add\n"
                          << "  orders past the cap and see the overload\n"
                          << "  rejection in action.\n";
                break;
            }
            case 9: om.displaySummary();             break;
            case 0: back = true;                     break;
            default: std::cout << "  Invalid option.\n"; break;
        }
    }
}


// Module 2 (Robot Assignment) sub menu
static void menuModule2(RobotAssignment& scheduler) {
    bool back = false;
    while (!back) {
        std::cout << "\nModule 2: Robot Assignment\n"
                  << "1. View robot roster\n"
                  << "2. View assignment history\n"
                  << "0. Back to main menu\n";
        int choice = readInt("Select: ");
        switch (choice) {
            case 1: scheduler.displayRobots();            break;
            case 2: scheduler.displayAssignmentHistory(); break;
            case 0: back = true;                          break;
            default: std::cout << "  Invalid option.\n"; break;
        }
    }
}


// Module 3 (Robot Navigation and Path Tracking) sub menu
static void menuModule3(RobotAssignment& scheduler, ItemBST& items) {
    bool back = false;
    while (!back) {
        std::cout << "\nModule 3: Robot Navigation\n"
                  << "1. Dispatch a robot to a target node\n"
                  << "2. View warehouse nodes (0 to 11)\n"
                  << "3. View robot positions\n"
                  << "0. Back to main menu\n";
        int choice = readInt("Select: ");
        switch (choice) {
            case 1: {
                std::string robotId = readLine("  Robot ID (e.g. R01): ");
                int target = readInt("  Target node (0-11)  : ");
                if (target < 0 || target > 11) {
                    std::cout << "  Target must be between 0 and 11.\n";
                    break;
                }
                Robot* r = scheduler.findRobotById(robotId);
                if (r == nullptr) {
                    std::cout << "  Robot not found.\n";
                    break;
                }
                
                if (!scheduler.recordManualAssignment("MANUAL", robotId, target))
                {
                    break;
                }
                
                std::cout << "  Picked " << robotId << " for manual dispatch.\n";
                
                r->assignTarget(target, true);
                scheduler.markRobotAvailable(robotId);
                break;
            }
            case 2: {
                std::cout << "  Warehouse has 12 nodes (0-11).\n"
                          << "  Items mapped per node (from Module 4):\n";
                items.displayAllItems();
                break;
            }
            case 3: scheduler.displayRobots();          break;
            case 0: back = true;                        break;
            default: std::cout << "  Invalid option.\n"; break;
        }
    }
}

// Module 4 (Item Search and Management) sub menu
static void menuModule4(ItemBST& items) {
    bool back = false;
    while (!back) {
        std::cout << "\nModule 4: Item Search\n"
                  << "1. View all items (BST in-order)\n"
                  << "2. Search by item ID\n"
                  << "3. Search by item name\n"
                  << "0. Back to main menu\n";
        int choice = readInt("Select: ");
        switch (choice) {
            case 1: items.displayAllItems(); break;
            case 2: {
                int id = readInt("  Numeric item ID (e.g. 12): ");
                items.searchByID(id);
                break;
            }
            case 3: {
                std::string name = readLine("  Item name: ");
                items.searchByName(name);
                break;
            }
            case 0: back = true; break;
            default: std::cout << "  Invalid option.\n"; break;
        }
    }
}

// Main entry point
int main() {
    std::cout << "Warehouse Robot Navigation System\n"
              << "Team: Roger, Isa, Delfino, Ryan\n\n";

    Warehouse       warehouse;
    OrderManager    om;
    RobotAssignment scheduler;
    ItemBST         items;

    int loadedOrders = om.loadFromCSV(DATA_FILE);
    int loadedRobots = scheduler.loadRobotsFromCSV(DATA_FILE, warehouse);
    int loadedItems  = loadItemsFromCSV(items, DATA_FILE);
    std::cout << "Loaded " << loadedOrders << " orders, "
              << loadedRobots << " robots, "
              << loadedItems  << " items from " << DATA_FILE << "\n";

    bool running = true;
    while (running) {
        std::cout << "\nMain Menu\n"
                  << "1. Module 1 - Order Management\n"
                  << "2. Module 2 - Robot Assignment\n"
                  << "3. Module 3 - Robot Navigation\n"
                  << "4. Module 4 - Item Search\n"
                  << "0. Exit\n";
        int choice = readInt("Select: ");
        switch (choice) {
            case 1: menuModule1(om, scheduler, items); break;
            case 2: menuModule2(scheduler);        break;
            case 3: menuModule3(scheduler, items); break;
            case 4: menuModule4(items);            break;
            case 0:
                running = false;
                std::cout << "Exiting.\n";
                break;
            default: std::cout << "  Invalid option.\n"; break;
        }
    }
    return 0;
}
