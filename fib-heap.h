/*
 * fib-heap.h
 *
 * By Randy Dang
 * Written December 2021 - January 2022
 * 
 * Interface for the Fibonacci Heap Data Structure. Useful structure for 
 * storing information in a way that allows for the following runtimes:
 *    BUILD EMPTY HEAP - O(1)
 *    BUILD HEAP FROM ARRAY - O(n)
 *    RETRIEVE MIN ELEMENT - O(1)
 *    REMOVE MIN ELEMENT - O(log n) amortized
 *    INSERT ELEMENT - O(1)
 *    DECREASE VALUE - O(1) amortized
 *    INCREASE VALUE - O(log n) amortized
 *    DELETE VALUE - O(log n) amortized
 *    GET VALUE FROM ADDRESS - O(1)
 *    GET ADDRESS FROM VALUE - O(n)
 * 
 * The Node struct definition is left public to allow the client to store 
 * node addresses as desired. This could be useful for implementing 
 * Dijkstra's Algorithm, as the Fibonacci heap node address storing 
 * the weight of an edge can be directly stored in an adjacency list. This 
 * does, however, allow the client to make direct modifications to the 
 * node data members. This interface and implementation assumes that the 
 * client will NOT modify any data members directly and will only store 
 * node addresses and pass node addresses back into functions of this class.
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

typedef int ElemType;

/* A node in the fibonacci heap storing an element. 
 *
 * NOTE: It is expected that the client will be well-behaved 
 *       and ONLY use returned Node * to store addresses and pass 
 *       back into functions of this class.
 *
 * value = element stored in node
 * loser = has node lost a child?
 * parent = pointer to node's parent in tree; nullptr if root
 * childIndex = index of node in parent's array of children; -1 if root
 * numChildren = number of children current node has
 */
struct Node {
    ElemType value;

    bool loser;
    Node *parent;
    int childIndex;
    std::vector<Node *> children;
    int numChildren;
};

class FibHeap {
    public:
        // Constructors, Destructor, Assignment Operator Overload
        FibHeap();
        FibHeap(ElemType *arr, int size);
        ~FibHeap();
        FibHeap(FibHeap &other);
        FibHeap &operator =(FibHeap &rhs);

        // Retrieve information
        bool isEmpty();
        int size();
        ElemType get_min();
        ElemType get_value(Node *node);
        Node *get_address(ElemType value);
        
        // Modify fibonacci heap
        Node *insert(ElemType value);
        ElemType remove_min();
        void decrease_val(Node *node, ElemType value);
        void delete_node(Node *node);
        Node *change_val(Node *node, ElemType value);
        void merge(FibHeap &other);
        void clear();

        // Print contents of fibonacci heap
        void print();

        // Checks if heap is valid, i.e. does not violate invariants
        bool valid();

    private:
        // Stores pointers to roots of trees in a ring structure
        struct RingNode {
            Node *root;
            RingNode *left;
            RingNode *right;
        };
        
        // Fibonacci heap data members
        RingNode *front;
        RingNode *min;
        int numElems;
        size_t maxDegree;

        // Map between tree roots and ringnodes pointing to them
        std::unordered_map<Node *, RingNode *> roots;

        // Helper functions
        Node *newNode(ElemType value);
        void delete_subtree(Node *root);
        Node *copy_subtree(Node *root);
        RingNode *merge_trees(RingNode *r_tree1, RingNode *r_tree2);
        void add_root(Node *root);
        void remove_ringnode(RingNode *ringNode);
        void copy_instance(FibHeap &other);
        Node *find_in_subtree(Node *node, ElemType value);

        // Helper functions for printing aspects of the fibonacci heap
        void print_subtree(Node *root);
        void print_node(Node *node);
        void print_value(Node *node);
        void print_values(std::vector<Node *> nodes);
        void print_bool(bool tf);

        // Helper functions for checking if heap is valid
        bool valid_ringnode(RingNode *ringnode, int *countElems);
        bool valid_subtree(Node *node, int *countElems, bool is_root);
};