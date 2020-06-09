//============================================================================
// Name        : HashTable.cpp
// Author      : Jef DeWitt
// Date        : 6.5.20
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <algorithm>
#include <climits>
#include <iostream>
#include <string> // atoi
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

const unsigned int DEFAULT_SIZE = 179;

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

//============================================================================
// Hash Table class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a hash table with chaining.
 */
class HashTable {

private:
    // Define structures to hold bids
    struct Node {
        Bid bid;
        unsigned key;
        Node* next; // pointer for linked list-izing node collisions

        Node() {
            key = UINT_MAX;
            next = nullptr;
        }

        // Init a new Node with a Bid
        Node(Bid t_Bid) : Node() { // Calls default constructor first
            bid = t_Bid;
        }

        // Init a new Node with a Bid and a key
        Node(Bid t_Bid, unsigned t_key) : Node(t_Bid) { // Calls custom constructor on line 63
            key = t_key;
        }
    };

    vector<Node> nodes;

    unsigned tableSize = DEFAULT_SIZE;

    unsigned int hash(int key);

public:
    HashTable();
    HashTable(unsigned size);
    virtual ~HashTable();
    void Insert(Bid bid);
    void PrintAll();
    void Remove(string bidId);
    Bid Search(string bidId);
};

/**
 * Default constructor
 */
HashTable::HashTable() {
    nodes.resize(tableSize); // resize structure used to hold bids
}

/**
 * Custom constructor allowing dynamic size
 * @param size
 */
HashTable::HashTable(unsigned size) {
    this->tableSize = size;
    nodes.resize(tableSize);
}

/**
 * Destructor
 */
HashTable::~HashTable() {
    nodes.erase(nodes.begin()); // free storage when class is destroyed
}

/**
 * Calculate the hash value of a given key.
 * Note that key is specifically defined as
 * unsigned int to prevent undefined results
 * of a negative list index.
 *
 * @param key The key to hash
 * @return The calculated hash
 */
unsigned int HashTable::hash(int key) {
    return key % tableSize; // calculate a hash value dynamically based on size of table
}

/**
 * Insert a bid
 *
 * @param bid The bid to insert
 */
void HashTable::Insert(Bid bid) {

    unsigned key = hash(atoi(bid.bidId.c_str())); // convert string object bidId to int

    Node* oldNode = &(nodes.at(key)); // Attempt to get node from key

    if(oldNode == nullptr) { // node isn't found
        Node* newNode = new Node(bid, key);
        nodes.insert(nodes.begin() + key, (*newNode)); // insert address of new node into table
    }
    else {
        if (oldNode->key == UINT_MAX) { // node is found
            oldNode->key = key;
            oldNode->bid = bid;
            oldNode->next = nullptr;
        }
        else {
            while (oldNode->next != nullptr) { // find the next open node
                oldNode = oldNode->next;
            }
            oldNode->next = new Node(bid, key);
        }
    }
}

/**
 * Print all bids
 */
void HashTable::PrintAll() {

    Node* node;
    Bid bid;

    for (unsigned i = 0; i < nodes.size(); ++i) {

        node = &nodes.at(i); // memory address of node at index
        if (node->key != UINT_MAX) { // does node contain bid?
            cout << node->bid.bidId << ": " << node->bid.title << " | " << node->bid.amount << " | "
               << node->bid.fund << endl;

            while (node->next != nullptr) { // is there a linked list attached?
                bid = node->next->bid;
                cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
                     << bid.fund << endl;
                node = node->next; // continue traversing linked list
            }
        }
    }
}

/**
 * Remove a bid
 *
 * @param bidId The bid id to search for
 */
void HashTable::Remove(string bidId) {
    // Implement logic to remove a bid
    unsigned key = hash(atoi(bidId.c_str()));
    nodes.erase(nodes.begin() + key); // add key for offset from first position
}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid HashTable::Search(string bidId) {
    Bid bid;

    unsigned key = hash(atoi(bidId.c_str())); // Calculate key

    Node* node = &(nodes.at(key)); // Attempt to get node from key

    // If node isn't found
    if (node == nullptr || node->key == UINT_MAX) {
        return bid;
    }

    // If node matches key
    if (node != nullptr && node->key != UINT_MAX
            && node->bid.bidId.compare(bidId) == 0) {
        return node->bid;
    }

    // traverse linked list for match
    while (node != nullptr) {
        if (node->key != UINT_MAX && node->bid.bidId.compare(bidId) == 0) {
            return node->bid;
        }
        node = node->next;
    }

    return bid;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information to the console (std::out)
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount << " | "
            << bid.fund << endl;
    return;
}

/**
 * Load a CSV file containing bids into a container
 *
 * @param csvPath the path to the CSV file to load
 * @return a container holding all the bids read
 */
void loadBids(string csvPath, HashTable* hashTable) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser using the given path
    csv::Parser file = csv::Parser(csvPath);

    // read and display header row - optional
    vector<string> header = file.getHeader();
    for (auto const& c : header) {
        cout << c << " | ";
    }
    cout << "" << endl;

    try {
        // loop to read rows of a CSV file
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // Create a data structure and add to the collection of bids
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

            // push this bid to the end
            hashTable->Insert(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 */
int main(int argc, char* argv[]) {

    // process command line arguments
    string csvPath, searchValue;
    switch (argc) {
    case 2:
        csvPath = argv[1];
        searchValue = "98109";
        break;
    case 3:
        csvPath = argv[1];
        searchValue = argv[2];
        break;
    default:
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        searchValue = "98109";
    }

    // Define a timer variable
    clock_t ticks;

    // Define a hash table to hold all the bids
    HashTable* bidTable;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Load Bids" << endl;
        cout << "  2. Display All Bids" << endl;
        cout << "  3. Find Bid" << endl;
        cout << "  4. Remove Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            bidTable = new HashTable();

            // Initialize a timer variable before loading bids
            ticks = clock();

            // Complete the method call to load the bids
            loadBids(csvPath, bidTable);

            // Calculate elapsed time and display result
            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 2:
            bidTable->PrintAll();
            break;

        case 3:
            ticks = clock();

            bid = bidTable->Search(searchValue);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
                cout << "Bid Id " << searchValue << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;
            break;

        case 4:
            bidTable->Remove(searchValue);
            break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
