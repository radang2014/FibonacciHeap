/*
 * fib-heap.cpp
 *
 * By Randy Dang
 * Written December 2021 - January 2022
 * 
 * Implementation for the Fibonacci Heap data structure. Useful for 
 * operations such as Dijkstra's Algorithm to allow for constant 
 * amortized time to decrease a value
 */

#include "fib-heap.h"

using namespace std;

// default constructor
FibHeap::FibHeap()
{
    front = nullptr;
    min = nullptr;
    numElems = 0;
    maxDegree = 1;
}

/* 
 * Secondary constructor; constructs a 
 * fibonacci heap using elements in inputted
 * array. Inputted size should be the size 
 * of the array.
 */
FibHeap::FibHeap(FibHeap_ElemType *arr, int size)
{
    front = nullptr;
    min = nullptr;
    numElems = 0;
    maxDegree = 1;

    for (int i = 0; i < size; i++) {
        insert(arr[i]);
    }
}

// destructor
FibHeap::~FibHeap()
{
    clear();
}

// copy constructor -- performs deep copy
FibHeap::FibHeap(FibHeap &other)
{
    front = nullptr;
    min = nullptr;
    numElems = 0;
    maxDegree = 1;
    
    copy_instance(other);
}

// assignment overload operator
FibHeap &FibHeap::operator =(FibHeap &rhs)
{
    if (this != &rhs) {
        clear();

        copy_instance(rhs);
    }
    return *this;
}

/* 
 * Returns whether or not fibonacci heap is empty
 */
bool FibHeap::isEmpty()
{
    return front == nullptr;
}

/*
 * Returns the number of elements in the fibonacci heap
 */
int FibHeap::size()
{
    return numElems;
}

/*
 * Retrieves the value of the minimum element in the fibonacci heap
 */
FibHeap_ElemType FibHeap::get_min()
{
    if (isEmpty()) {
        cerr << "Heap is empty -- cannot get the minimum element" << endl;
        exit(EXIT_FAILURE);
    }
    return min->root->value;
}

/*
 * Retrieves the value stored at a specific address
 */
FibHeap_ElemType FibHeap::get_value(FibHeap_ElemAddr addr)
{
    Node *node = (Node *)addr;

    if (node == nullptr) {
        cerr << "Cannot get the value of a null node" << endl;
        exit(EXIT_FAILURE);
    }

    return node->value;
}

/*
 * Retrieves the address of inputted value in the heap. If address 
 * is not in the heap, returns nullptr. Note that this takes 
 * linear time in the worst case, and the runtime would be better
 * if addresses were instead stored by the client in the appropriate
 * structure.
 */
FibHeap_ElemAddr FibHeap::get_address(FibHeap_ElemType value)
{
    if (front != nullptr) {
        // Search in first tree
        Node *in_front = find_in_subtree(front->root, value);
        if (in_front != nullptr) {
            return in_front;
        }

        // Search in remaining trees
        for (RingNode *curr = front->right; curr != front; curr = curr->right) {
            Node *in_curr = find_in_subtree(curr->root, value);
            if (in_curr != nullptr) {
                return in_curr;
            }
        }
    }

    return nullptr;
}

/*
 * Inserts an element into the fibonacci heap and returns a pointer 
 * to the node storing that element.
 */
FibHeap_ElemAddr FibHeap::insert(FibHeap_ElemType value)
{
    // Insert value into root of a new tree
    Node *root = newNode(value);
    add_root(root);

    numElems++;
    return root;
}

FibHeap_ElemType FibHeap::remove_min()
{
    if (isEmpty()) {
        cerr << "Heap is empty -- cannot remove the minimum element" << endl;
        exit(EXIT_FAILURE);
    }

    FibHeap_ElemType old_min = get_min();

    // Promote all children of minimum to roots
    Node *minNode = min->root;
    for (size_t i = 0; i < minNode->children.size(); i++) {
        if (minNode->children[i] != nullptr) {
            add_root(minNode->children[i]);
        }
    }

    // Delete nodes storing previous minimum element
    numElems--;
    remove_ringnode(min);
    delete minNode;

    // Return to empty heap if no elements left
    if (numElems == 0) {
        front = nullptr;
        min = nullptr;
        return old_min;
    }

    // Merge trees until no two trees have the same degree
    // Do so using helper vector holding trees indexed by degree
    vector<RingNode *> trees_by_degree(maxDegree + 1, nullptr);
    trees_by_degree[front->root->numChildren] = front;
    for (RingNode *curr = front->right; curr != front; curr = curr->right) {
        int degree = curr->root->numChildren;
        while (trees_by_degree[degree] != nullptr and trees_by_degree[degree] != curr) {
            curr = merge_trees(curr, trees_by_degree[degree]);
            if (maxDegree >= trees_by_degree.size()) {
                trees_by_degree.push_back(nullptr);
            }

            trees_by_degree[degree] = nullptr;
            degree++;
        }
        trees_by_degree[degree] = curr;
    }

    // Locate node containing new minimum element and adjust pointer
    min = front;
    for (RingNode *curr = front->right; curr != front; curr = curr->right) {
        if (curr->root->value < min->root->value) {
            min = curr;
        }
    }

    return old_min;
}

/*
 * Decreases the value held at inputted node address to the 
 * inputted new value. The inputted new value is expected 
 * to be less than the value currently held at that address.
 */
void FibHeap::decrease_val(FibHeap_ElemAddr addr, FibHeap_ElemType value)
{
    Node *node = (Node *)addr;

    if (node == nullptr) {
        cerr << "Cannot decrease the value of a null node" << endl;
        exit(EXIT_FAILURE);
    }

    if (value >= node->value) {
        cerr << "ERROR: Can only decrease to a value lower than current value" << endl;
        exit(EXIT_FAILURE);
    }

    // Decrease value at node
    node->value = value;
    
    // If no heap invariants are violated, then update min if necessary and we are done.
    if (node->parent == nullptr or node->parent->value <= value) {
        if (value < get_min()) {
            min = roots[node];
        }
        return;
    }

    // Otherwise, move node with decreased value and subtree to a 
    // new tree and update information on if parent lost a child.
    // If parent loses two children, also move the parent to 
    // its own tree.
    Node *curr = node;
    Node *parent = node->parent;
    while (parent != nullptr) {
        parent->children[curr->childIndex] = nullptr;
        parent->numChildren--;
        add_root(curr);
        if (parent->loser) {
            curr = parent;
            parent = parent->parent;
        } else {
            if (parent->parent != nullptr) {
                parent->loser = true;
            }
            break;
        }
    }
}

/*
 * Deletes node at inputted address from heap
 */
void FibHeap::delete_elem(FibHeap_ElemAddr addr)
{
    Node *node = (Node *)addr;

    decrease_val(node, get_min() - 1);
    remove_min();
}

/* 
 * Changes the node at inputted address to a different 
 * value. The new value may be at a different address,
 * so the new address of the modified value is 
 * returned.
 */
void FibHeap::change_val(FibHeap_ElemAddr *addr_p, FibHeap_ElemType value)
{
    Node *node = (Node *)*addr_p;

    if (value < node->value) {
        decrease_val(node, value);
    } else if (value > node->value) {
        delete_elem(node);
        *addr_p = insert(value);
    }
}

/* 
 * Merges contents of this instance of a fibonacci heap with 
 * another instance, and empties contents of other instance.
 */
void FibHeap::merge(FibHeap &other)
{
    // Handle cases where either fibonacci heap is empty.
    if (other.isEmpty()) {
        return;
    }
    if (isEmpty()) {
        front = other.front;
        min = other.min;
        numElems = other.numElems;
        maxDegree = other.maxDegree;

        roots = other.roots;
    }
    // Handle cases where fibonacci heaps are non-empty.
    else {
        // Stitch two ringnodes together
        RingNode *back = front->left;
        RingNode *other_back = other.front->left;

        back->right = other.front;
        other.front->left = back;
        other_back->right = front;
        front->left = other_back;

        // Update data members
        for (auto itr = other.roots.begin(); itr != other.roots.end(); itr++) {
            roots.insert({itr->first, itr->second});
        }
        numElems += other.numElems;
        if (other.maxDegree > maxDegree) {
            maxDegree = other.maxDegree;
        }
    }

    // Set other instance to values for empty heap to avoid double frees
    other.front = nullptr;
    other.min = nullptr;
    other.numElems = 0;
    other.maxDegree = 2;
}

/*
 * Clears fibonacci heap of all elements
 */
void FibHeap::clear()
{
    if (not isEmpty()) {
        RingNode *curr = front->left;
        while (curr != front) {
            delete_subtree(curr->root);
            curr = curr->left;
            delete curr->right;
        }
        delete_subtree(front->root);
        delete front;

        front = nullptr;
        min = nullptr;
        numElems = 0;
        maxDegree = 2;
    }
}

/*
 * Prints contents of fibonacci heap
 */
void FibHeap::print()
{    
    // Print contents of heap
    int count = 1;
    if (not isEmpty()) {
        cout << "FRONT: "; print_value(front->root); cout << endl;
        cout << "MIN: "; print_value(min->root); cout << endl;
        cout << "NUMELEMS: " << numElems << endl;
        cout << "MAXDEGREE: " << maxDegree << endl;
        cout << endl;
        
        cout << "TREE " << count << ":" << endl;
        cout << "Root: "; print_value(front->root); cout << endl;
        cout << "Left: "; print_value(front->left->root); cout << endl;
        cout << "Right: "; print_value(front->right->root); cout << endl;
        cout << endl;
        print_subtree(front->root);
        count++;
        for (RingNode *curr = front->right; curr != front; curr = curr->right) {
            cout << endl;
            cout << "TREE " << count << ":" << endl;
            cout << "Root: "; print_value(curr->root); cout << endl;
            cout << "Left: "; print_value(curr->left->root); cout << endl;
            cout << "Right: "; print_value(curr->right->root); cout << endl;
            cout << endl;
            print_subtree(curr->root);
            count++;
        }
    }
}

/* 
 * Create new node holding inputted value 
 * and return pointer to that node 
 */
FibHeap::Node *FibHeap::newNode(FibHeap_ElemType value)
{
    Node *result = new Node;
    result->value = value;

    result->loser = false;
    result->parent = nullptr;
    result->childIndex = -1;
    result->numChildren = 0;

    return result;
}

/* 
 * Deletes subtree starting at inputted node (node + all descendents)
 */
void FibHeap::delete_subtree(Node *root)
{
    if (root != nullptr) {
        for (size_t i = 0; i < root->children.size(); i++) {
            delete_subtree(root->children[i]);
        }
        delete root;
    }
}

/*
 * Copy subtree starting at inputted node and return pointer to copy
 */
FibHeap::Node *FibHeap::copy_subtree(Node *root)
{
    if (root == nullptr) {
        return nullptr;
    }
    Node *root_cpy = newNode(root->value);
    root_cpy->loser = root->loser;
    root_cpy->childIndex = root->childIndex;
    for (size_t i = 0; i < root->children.size(); i++) {
        root_cpy->children.push_back(copy_subtree(root->children[i]));
        if (root_cpy->children[i] != nullptr) {
            root_cpy->children[i]->parent = root_cpy;
        }
    }
    root_cpy->numChildren = root->numChildren;

    return root_cpy;
}

/*
 * Merge inputted tree1 and tree2 in constant time and return result.
 * It is expected that both inputted RingNodes are valid nodes in the 
 * ring structure that point to non-empty trees.
 */
FibHeap::RingNode *FibHeap::merge_trees(RingNode *r_tree1, RingNode *r_tree2)
{
    Node *tree1 = r_tree1->root;
    Node *tree2 = r_tree2->root;
    
    // Have tree with smaller root adopt tree with larger root
    if (tree1->value <= tree2->value) {
        tree1->children.push_back(tree2);
        tree1->numChildren++;
        tree2->childIndex = tree1->children.size() - 1;
        tree2->parent = tree1;
        if (tree1->children.size() > maxDegree) {
            maxDegree = tree1->children.size();
        }

        // Remove adopted root from ring structure, as it is no 
        // longer a root.
        remove_ringnode(r_tree2);

        return r_tree1;
    } else {
        return merge_trees(r_tree2, r_tree1);
    }
    
}

/* 
 * Add inputted node and its descendents as a root to the 
 * fibonacci heap.
 */
void FibHeap::add_root(Node *root)
{
    // Create new tree on ring to hold root
    RingNode *ringNode = new RingNode;
    ringNode->root = root;
    root->loser = false;
    root->parent = nullptr;
    root->childIndex = -1;

    // Case where original heap is empty; create a new tree
    if (isEmpty()) {
        front = ringNode;
        ringNode->right = ringNode;
        ringNode->left = ringNode;
        min = ringNode;
    } 
    // Otherwise, insert before minimum and update minimum if needed
    else {
        ringNode->right = min;
        ringNode->left = min->left;
        min->left->right = ringNode;
        min->left = ringNode;

        if (root->value < get_min()) {
            min = ringNode;
        }
    }

    roots.insert({root, ringNode});
}

/* 
 * Remove inputted ringNode from the fibonacci heap structure without
 * deallocating memory in the actual tree
 */
void FibHeap::remove_ringnode(RingNode *ringNode)
{
    if (ringNode != nullptr) {
        roots.erase(ringNode->root);
        if (ringNode == ringNode->left) {
            delete ringNode;
            front = nullptr;
            min = nullptr;
        } else {
            if (ringNode == front) {
                front = ringNode->right;
            }
            ringNode->left->right = ringNode->right;
            ringNode->right->left = ringNode->left;
            delete ringNode;
        }
    }
}

/*
 * Performs a deep copy of another instance and sets current 
 * instance equal to that copy. This function does NOT 
 * deallocate any memory currently allocated to this instance
 */
void FibHeap::copy_instance(FibHeap &other)
{
    if (not other.isEmpty()) {
        add_root(copy_subtree(other.front->root));
        for (RingNode *curr = other.front->right; curr != other.front; curr = curr->right) {
            add_root(copy_subtree(curr->root));
        }
        numElems = other.numElems;
        maxDegree = other.maxDegree;
    }
}

/*
 * Searches in subtree of inputted node for value. Returns address if value exists,
 * and returns nullptr if value does not exist.
 */
FibHeap::Node *FibHeap::find_in_subtree(Node *node, FibHeap_ElemType value)
{
    if (node != nullptr and node->value <= value) {
        if (node->value == value) {
            return node;
        }
        for (size_t i = 0; i < node->children.size(); i++) {
            Node *in_subtree = find_in_subtree(node->children[i], value);
            if (in_subtree != nullptr) {
                return in_subtree;
            }
        }
    }
    return nullptr;
}

/* 
 * Prints contents of one subtree in level order
 */
void FibHeap::print_subtree(Node *root)
{
    if (root == nullptr) {
        return;
    }
    
    queue<Node *> to_print;
    to_print.push(root);

    while (not to_print.empty()) {
        Node *curr = to_print.front();
        to_print.pop();

        print_node(curr);

        for (size_t i = 0; i < curr->children.size(); i++) {
            Node *child = curr->children[i];
            if (child != nullptr) {
                to_print.push(child);
            }
        }
    }
}

/*
 * Prints all information stored in inputted node
 */
void FibHeap::print_node(Node *node)
{
    cout << "NODE: " << endl;
    if (node != nullptr) {
        cout << "Value: " << node->value << endl;
        cout << "Loser: "; print_bool(node->loser); cout << endl;
        cout << "Parent: "; print_value(node->parent); cout << endl;
        cout << "ChildIndex: " << node->childIndex << endl;
        cout << "Children: "; print_values(node->children); cout << endl;
        cout << "NumChildren: " << node->numChildren << endl;
    }
    cout << endl;
}

/*
 * Prints value stored in a node, or null if node is null
 */
void FibHeap::print_value(Node *node) {
    if (node == nullptr) {
        cout << "null";
    } else {
        cout << node->value;
    }
}

/*
 * Prints comma separated list of values stored in list of nodes
 */
void FibHeap::print_values(std::vector<Node *> nodes)
{
    bool first = true;
    for (size_t i = 0; i < nodes.size(); i++) {
        if (not first) {
            cout << ", ";
        }
        print_value(nodes[i]);
        first = false;
    }
}

void FibHeap::print_bool(bool tf) {
    if (tf) {
        cout << "T";
    } else {
        cout << "F";
    }
}

/*
 * Returns whether or not heap is valid (does not violate heap invariants) and 
 * prints an error message if this is not the case
 */
bool FibHeap::valid() {
    int countElems = 0;
    if (front != nullptr) {
        if (min == nullptr or min->root == nullptr) {
            cerr << "ERROR: Heap is not empty but the minimum pointer is null" << endl;
            return false;
        }
        if (not valid_ringnode(front, &countElems)) {
            return false;
        }
        for (RingNode *curr = front->right; curr != front; curr = curr->right) {
            if (not valid_ringnode(curr, &countElems)) {
                return false;
            }
        }
    }
    if (countElems != numElems) {
        cerr << "ERROR: It is reported that there are " << numElems 
             << " elements in the heap when there are actually " 
             << countElems << " elements." << endl;
        return false;
    }
    return true;
}

/*
 * Returns whether or not ringNode and tree pointed to is valid.
 * Also increments countElems by the number of nodes in tree 
 * pointed to by ringnode. Prints an error message if this is not
 * the case.
 */
bool FibHeap::valid_ringnode(RingNode *ringnode, int *countElems) {
    if (ringnode != nullptr) {
        if (ringnode->root == nullptr) {
            cerr << "ERROR: Ringnode points to an empty tree" << endl;
            return false;
        }
        if (ringnode->root->value < min->root->value) {
            cerr << "ERROR: Minimum points to " << min->root->value << " while " 
                 << ringnode->root->value << " exists." << endl;
            return false;
        }
        if (roots[ringnode->root] != ringnode) {
            cerr << "ERROR: Tree with root storing " << ringnode->root->value 
                 << " is mapped to ringnode storing " << roots[ringnode->root]->root->value << endl;
            return false;
        }
        if (not valid_subtree(ringnode->root, countElems, true)) {
            return false;
        }
    }

    return true;
}

/*
 * Returns whether or not subtree with inputted node as root is valid, or does not violate
 * heap invariants.  Also increments countElems by the number of nodes in tree 
 * pointed to by ringnode. Prints an error message if this is not
 * the case.
 */
bool FibHeap::valid_subtree(Node *node, int *countElems, bool is_root) {
    if (node != nullptr) {
        if (is_root) {
            if (node->parent != nullptr or node->childIndex != -1) {
                cerr << "ERROR: Node storing " << node->value 
                     << " is a root but does not follow root invariants." << endl;
                return false;
            }
        } else {
            if (node->parent == nullptr) {
                cerr << "ERROR: Node storing " << node->value << " is not a root but has no parent" << endl;
                return false;
            }
            if ((int)node->parent->children.size() <= node->childIndex) {
                cerr << "ERROR: Node storing " << node->value << " has a child index of " << node->childIndex 
                     << " but parent list only has room for " << node->parent->children.size() << " children." << endl;
                return false;
            }
            if (node->parent->children[node->childIndex] != node) {
                cerr << "ERROR: Node storing " << node->value << " has a child index of " << node->childIndex
                     << " but parent list at that index points to node storing ";
                print_value(node->parent->children[node->childIndex]);
                cerr << endl;
                return false;
            }
            if (roots.count(node) != 0) {
                cerr << "ERROR: Node storing " << node->value << " is stored as a root but is not a root." << endl;
                return false;
            }
        }

        int numChildren = 0;
        for (size_t i = 0; i < node->children.size(); i++) {
            if (node->children[i] != nullptr) {
                if (node->children[i]->parent != node) {
                    cerr << "ERROR: Node storing " << node->value << " has a child storing " << node->children[i]->value 
                         << " whose parent is not pointing to the original node." << endl;
                    return false;
                }
                if (node->children[i]->childIndex != (int)i) {
                    cerr << "ERROR: Node storing " << node->value << " has a child storing " << node->children[i]->value
                         << " at index " << i << " but the child is storing index " << node->children[i]->childIndex
                         << " instead." << endl;
                    return false;
                }
                if (node->children[i]->value < node->value) {
                    cerr << "ERROR: Node storing " << node->value << " has a child storing " << node->children[i]->value
                         << ", violating min heap invariants." << endl;
                    return false;
                }
                if (not valid_subtree(node->children[i], countElems, false)) {
                    return false;
                }
                numChildren++;
            }
        }
        if (numChildren != node->numChildren) {
            cerr << "ERROR: Node storing " << node->value << " has " << numChildren << " children but is reporting "
                 << node->numChildren << " children." << endl;
            return false;
        }

        (*countElems)++;
    }

    return true;
}




