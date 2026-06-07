// Module4.hpp (Item Search and Management) 
// Ryan - TP082108

#ifndef MODULE4_HPP
#define MODULE4_HPP

#include "Common.hpp"
#include <string>


// ItemNode - one node in the BST.
struct ItemNode {
    int         itemID;    
    std::string itemName;   
    std::string location;  
    int         graphNode;  
    ItemNode*   left;
    ItemNode*   right;

    ItemNode(int id, const std::string& name,
             const std::string& loc, int gNode);
};


// ItemBST - binary search tree keyed by itemID.
class ItemBST {
private:
    ItemNode* root;

    ItemNode* insertHelper      (ItemNode* node, int id, const std::string& name,
                                 const std::string& loc, int gNode, bool silent);
    ItemNode* searchByIDHelper  (ItemNode* node, int id) const;
    void      searchByNameHelper(ItemNode* node, const std::string& name,
                                 bool& found) const;
    ItemNode* findMinNode       (ItemNode* node) const;
    ItemNode* deleteHelper      (ItemNode* node, int id, bool& deleted);
    void      inOrderHelper     (ItemNode* node) const;
    void      destroyTree       (ItemNode* node);

public:
    ItemBST();
    ~ItemBST();

    ItemBST(const ItemBST&)            = delete;
    ItemBST& operator=(const ItemBST&) = delete;

    void insertItem  (int id, const std::string& name,
                      const std::string& loc, int gNode,
                      bool silent = false);
    void searchByID  (int id) const;
    void searchByName(const std::string& name) const;
    void updateItem  (int id, const std::string& newName,
                      const std::string& newLoc);
    void deleteItem  (int id);
    void displayAllItems() const;
    bool isEmpty()         const;

    int  findLocationNode(int id) const;
    int  findLocationNodeByString(const std::string& itemIdStr) const;
};


// Free function helpers
int parseItemID(const std::string& itemIDStr);

int loadItemsFromCSV(ItemBST& bst, const std::string& filename);

#endif 
