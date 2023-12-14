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
 * FibHeap_ElemAddr is a type given to the client to allow for storing of 
 * element addresses as desired. This could be useful for implementing 
 * Dijkstra's Algorithm, as the Fibonacci heap address storing 
 * the weight of an edge can be directly stored in an adjacency list. 
 * In order for this to work, the client should NOT modify anything pointed 
 * to by the address directly; addresses will only be used as handles to 
 * be passed back into functions of this class. 
 */

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>

typedef int FibHeap_ElemType;
typedef void *FibHeap_ElemAddr;

class FibHeap {
    public:
        // Constructors, Destructor, Assignment Operator Overload
        FibHeap();
        FibHeap(FibHeap_ElemType *arr, int size);
        ~FibHeap();
        FibHeap(FibHeap &other);
        FibHeap &operator =(FibHeap &rhs);

        // Retrieve information
        bool isEmpty();
        int size();
        FibHeap_ElemType get_min();
        FibHeap_ElemType get_value(FibHeap_ElemAddr addr);
        FibHeap_ElemAddr get_address(FibHeap_ElemType value);
        
        // Modify fibonacci heap
        FibHeap_ElemAddr insert(FibHeap_ElemType value);
        FibHeap_ElemType remove_min();
        void decrease_val(FibHeap_ElemAddr addr, FibHeap_ElemType value);
        void delete_elem(FibHeap_ElemAddr addr);
        void change_val(FibHeap_ElemAddr *addr_p, FibHeap_ElemType value);
        void merge(FibHeap &other);
        void clear();

        // Print contents of fibonacci heap
        void print();

        // Checks if heap is valid, i.e. does not violate internal invariants
        bool valid();

    private:
        /* A node in the fibonacci heap storing an element: 
         * value = element stored in node
         * loser = has node lost a child?
         * parent = pointer to node's parent in tree; nullptr if root
         * childIndex = index of node in parent's array of children; -1 if root
         * numChildren = number of children current node has
         */
        struct Node {
            FibHeap_ElemType value;

            bool loser;
            Node *parent;
            int childIndex;
            std::vector<Node *> children;
            int numChildren;
        };

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
        Node *newNode(FibHeap_ElemType value);
        void delete_subtree(Node *root);
        Node *copy_subtree(Node *root);
        RingNode *merge_trees(RingNode *r_tree1, RingNode *r_tree2);
        void add_root(Node *root);
        void remove_ringnode(RingNode *ringNode);
        void copy_instance(FibHeap &other);
        Node *find_in_subtree(Node *node, FibHeap_ElemType value);

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