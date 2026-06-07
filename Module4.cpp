// Module4.cpp (Item Search and Management)
// Ryan Stefanosky Lim - TP082108

#include "Module4.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>


// ItemNode
ItemNode::ItemNode(int id, const std::string& name,
                   const std::string& loc, int gNode)
    : itemID(id), itemName(name), location(loc),
      graphNode(gNode), left(nullptr), right(nullptr) {}


// ItemBST - lifecycle
ItemBST::ItemBST() : root(nullptr) {}

ItemBST::~ItemBST() { destroyTree(root); }

void ItemBST::destroyTree(ItemNode* node) {
    if (node == nullptr) return;
    destroyTree(node->left);
    destroyTree(node->right);
    delete node;
}

bool ItemBST::isEmpty() const {
    return root == nullptr;
}


// Recursive helpers
ItemNode* ItemBST::insertHelper(ItemNode* node, int id,
                                const std::string& name,
                                const std::string& loc,
                                int gNode, bool silent) {
    if (node == nullptr) {
        return new ItemNode(id, name, loc, gNode);
    }
    if (id < node->itemID) {
        node->left  = insertHelper(node->left,  id, name, loc, gNode, silent);
    } else if (id > node->itemID) {
        node->right = insertHelper(node->right, id, name, loc, gNode, silent);
    } else {
        if (!silent) {
            std::cout << "  [Module 4] Item ID "
                      << id << " already exists.\n";
        }
    }
    return node;
}

ItemNode* ItemBST::searchByIDHelper(ItemNode* node, int id) const {
    if (node == nullptr || node->itemID == id) return node;
    if (id < node->itemID) return searchByIDHelper(node->left,  id);
    else                   return searchByIDHelper(node->right, id);
}

void ItemBST::searchByNameHelper(ItemNode* node, const std::string& name,
                                 bool& found) const {
    if (node == nullptr) return;
    if (node->itemName == name) {
        std::cout << "  Item ID   : " << node->itemID   << "\n"
                  << "  Name      : " << node->itemName << "\n"
                  << "  Location  : " << node->location << "\n"
                  << "  Graph node: " << node->graphNode << "\n";
        found = true;
    }
    searchByNameHelper(node->left,  name, found);
    searchByNameHelper(node->right, name, found);
}

ItemNode* ItemBST::findMinNode(ItemNode* node) const {
    while (node != nullptr && node->left != nullptr) {
        node = node->left;
    }
    return node;
}


ItemNode* ItemBST::deleteHelper(ItemNode* node, int id, bool& deleted) {
    if (node == nullptr) return nullptr;
    if (id < node->itemID) {
        node->left  = deleteHelper(node->left,  id, deleted);
    } else if (id > node->itemID) {
        node->right = deleteHelper(node->right, id, deleted);
    } else {
        deleted = true;

        // Case 1: leaf.
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        }
        // Case 2: one child.
        if (node->left == nullptr) {
            ItemNode* temp = node->right;
            delete node;
            return temp;
        }
        if (node->right == nullptr) {
            ItemNode* temp = node->left;
            delete node;
            return temp;
        }

        // Case 3: two children.
        ItemNode* successor = findMinNode(node->right);
        node->itemID    = successor->itemID;
        node->itemName  = successor->itemName;
        node->location  = successor->location;
        node->graphNode = successor->graphNode;
        node->right     = deleteHelper(node->right, successor->itemID, deleted);
    }
    return node;
}

void ItemBST::inOrderHelper(ItemNode* node) const {
    if (node == nullptr) return;
    inOrderHelper(node->left);
    std::cout << "  "
              << std::left << std::setw(4)  << node->itemID
              << std::left << std::setw(24) << node->itemName
              << std::left << std::setw(34) << node->location
              << "node " << node->graphNode << "\n";
    inOrderHelper(node->right);
}


// Public operations
void ItemBST::insertItem(int id, const std::string& name,
                         const std::string& loc, int gNode, bool silent) {
    root = insertHelper(root, id, name, loc, gNode, silent);
    if (!silent) {
        std::cout << "  Item added.\n"
                  << "  Item ID   : " << id    << "\n"
                  << "  Name      : " << name  << "\n"
                  << "  Location  : " << loc   << "\n"
                  << "  Graph node: " << gNode << "\n";
    }
}

void ItemBST::searchByID(int id) const {
    ItemNode* result = searchByIDHelper(root, id);
    if (result != nullptr) {
        std::cout << "  Item ID   : " << result->itemID    << "\n"
                  << "  Name      : " << result->itemName  << "\n"
                  << "  Location  : " << result->location  << "\n"
                  << "  Graph node: " << result->graphNode << "\n";
    } else {
        std::cout << "  Item ID " << id << " not found.\n";
    }
}

void ItemBST::searchByName(const std::string& name) const {
    bool found = false;
    searchByNameHelper(root, name, found);
    if (!found) std::cout << "  No item named \"" << name << "\" found.\n";
}

void ItemBST::updateItem(int id, const std::string& newName,
                         const std::string& newLoc) {
    ItemNode* result = searchByIDHelper(root, id);
    if (result != nullptr) {
        result->itemName = newName;
        result->location = newLoc;
        std::cout << "  Item updated.\n"
                  << "  Item ID   : " << id      << "\n"
                  << "  Name      : " << newName << "\n"
                  << "  Location  : " << newLoc  << "\n";
    } else {
        std::cout << "  Item ID " << id << " not found.\n";
    }
}

void ItemBST::deleteItem(int id) {
    bool deleted = false;
    root = deleteHelper(root, id, deleted);
    if (deleted) std::cout << "  Item ID " << id << " deleted successfully.\n";
    else         std::cout << "  Item ID " << id << " not found.\n";
}

void ItemBST::displayAllItems() const {
    if (root == nullptr) {
        std::cout << "  No items in the system.\n";
        return;
    }
    std::cout << "  "
              << std::left << std::setw(4)  << "ID"
              << std::left << std::setw(24) << "Name"
              << std::left << std::setw(34) << "Location"
              << "Graph\n";
    inOrderHelper(root);
}

int ItemBST::findLocationNode(int id) const {
    ItemNode* result = searchByIDHelper(root, id);
    if (result == nullptr) return -1;
    return result->graphNode;
}

int ItemBST::findLocationNodeByString(const std::string& itemIdStr) const {
    if (itemIdStr.size() < 5) return -1; 
    return findLocationNode(parseItemID(itemIdStr));
}


// Free-function helpers
int parseItemID(const std::string& itemIDStr) {
    if (itemIDStr.size() <= 4) return 0;
    std::string numPart = itemIDStr.substr(4);
    int id = 0;
    for (size_t i = 0; i < numPart.size(); i++) {
        char c = numPart[i];
        if (c < '0' || c > '9') break;
        id = id * 10 + (c - '0');
    }
    return id;
}


static int itemIDtoGraphNode(int itemID) {
    switch (itemID) {
        case 8:  return 0;  // USB-C Cable          Zone A
        case 12: return 0;  // Wireless Mouse       Zone A
        case 14: return 1;  // HDMI Cable           Zone A
        case 19: return 2;  // Laptop Stand         Zone A
        case 23: return 3;  // Bluetooth Keyboard   Zone A
        case 31: return 4;  // Desk Organizer       Zone B
        case 33: return 4;  // Sticky Notes         Zone B
        case 42: return 5;  // Ballpoint Pens       Zone B
        case 45: return 6;  // A4 Paper Ream        Zone B
        case 52: return 7;  // Stapler              Zone B
        case 55: return 8;  // Coffee Mug           Zone C
        case 67: return 9;  // Thermal Flask        Zone C
        case 76: return 10; // Notebook A5          Zone C
        case 89: return 11; // Highlighter Set      Zone D
        case 91: return 11; // Whiteboard Marker    Zone D
        default: return 0;  // default to loading bay
    }
}

int loadItemsFromCSV(ItemBST& bst, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "  [Module 4] Could not open " << filename << "\n";
        return 0;
    }

    std::string line;
    std::getline(file, line); 

    int count = 0;

    std::string lastItemId = "";

    while (std::getline(file, line)) {
        std::string fields[22];
        int         i = 0;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',') && i < 22) {
            fields[i++] = token;
        }

        const std::string& itemIDStr = fields[14];
        const std::string& itemName  = fields[15];
        const std::string& zone      = fields[17];
        const std::string& aisle     = fields[18];
        const std::string& shelf     = fields[19];

        if (itemIDStr.empty() || itemName.empty()) continue;
        if (itemIDStr == lastItemId)                continue; 
        lastItemId = itemIDStr;

        int id = parseItemID(itemIDStr);
        std::string location = "Zone " + zone +
                               " - Aisle " + aisle +
                               " - Shelf " + shelf;
        int gNode = itemIDtoGraphNode(id);

        bst.insertItem(id, itemName, location, gNode, true);
        count++;
    }

    file.close();
    return count;
}
