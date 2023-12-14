/*
 * use-heap-example.cpp
 * Written by Randy Dang, January 2022
 * 
 * Provides examples for ways a Fibonacci Heap can be used.
 * Note that Fibonacci Heaps are most useful for storing 
 * edge values to decrease the runtime of Dijkstra's Shortest
 * Path and Prim's Minimum Spanning Tree algorithms.
 * 
 * Some examples use assert() statements to show the expected
 * value that a function should return in the specific scenario.
 * 
 * Running this code will produce no output, indicating that 
 * all assert() statements passed.
 */

#include <iostream>
#include <unordered_map>
#include <string>
#include <cassert>

#include "fib-heap.h"

using namespace std;

int main() {
    /* 
     * It is up to the client to store Node addresses in an 
     * appropriate data structure. The examples below use an 
     * unordered map to map values to addresses. Depending 
     * on the purpose, the client may store addresses in a 
     * balanced tree, a struct representing a graph edge 
     * stored in adjacency list(s), etc. Alternatively, 
     * the client can call get_address to get the address
     * of a value, but that will cause operations to 
     * take at least linear time.
     */
    unordered_map<int, FibHeap_ElemAddr> address_map;

    /* Create an empty Fibonacci Heap */
    FibHeap heap1;

    /* 
     * Create a heap containing the same elements as an array 
     * containing numbers 1-10. Note that this method does not
     * return the addresses of any Nodes in the heap, so it 
     * is recommended to create an empty heap and use insert()
     * instead.
     */
    int nums[10];
    for (int i = 0; i < 10; i++) {
        nums[i] = i + 1;
    }
    FibHeap heap2(nums, 10);

    /*
     * Make a new heap (heap3) with the contents of an existing 
     * heap (heap2)
     */
    FibHeap heap3(heap2);

    /*
     * Set an existing heap (heap3) to have the contents of an 
     * existing heap (heap1)
     */
    heap3 = heap1;

    /* Check whether or not heap is empty */
    assert(heap1.isEmpty());
    assert(not heap2.isEmpty());
    assert(heap3.isEmpty());

    /* Check the number of elements in heap */
    assert(heap1.size() == 0);
    assert(heap2.size() == 10);
    assert(heap3.size() == 0);

    /* 
     * Get the minimum element in heap. If heap is empty,
     * the program quits with an exit code of 1.
     */
    assert(heap2.get_min() == 1);
    
    /* 
     * Get the address of a node with a certain value. The example
     * below gets the address of a node containing the value 4. Note 
     * that get_address takes linear time, so it is recommended
     * for clients to store addresses themselves in the appropriate
     * structure if possible.
     */
    FibHeap_ElemAddr addr = heap2.get_address(4);

    /* 
     * Get the value of a Node when given the address. The example
     * below gets the value of the node containing the value 4.
     */
    assert(heap2.get_value(addr) == 4);

    /* Insert elements into heap and store addresses in map */
    address_map.insert({3, heap1.insert(3)}); // inserts 3 into heap1
    address_map.insert({8, heap1.insert(8)}); // inserts 8 into heap1
    address_map.insert({1, heap1.insert(1)});
    address_map.insert({18, heap1.insert(18)});
    address_map.insert({13, heap1.insert(13)});
    address_map.insert({85, heap1.insert(85)});
    address_map.insert({-2, heap1.insert(-2)});
    address_map.insert({44, heap1.insert(44)});
    /* Check that address_map and the fibonacci heap are in sync */
    assert(heap1.get_address(3) == address_map[3]);
    assert(heap1.get_value(address_map[3]) == 3);

    /* Remove minimum element from heap */
    assert(heap1.remove_min() == -2);
    assert(heap1.remove_min() == 1);
    /* Update address_map to sync with state of heap */
    address_map.erase(-2);
    address_map.erase(1);

    /* 
     * Decrease the value 44 to the value 2. If the 
     * value passed in is not lower than the current 
     * value of the node, the program will exit with 
     * a code of 1
     */
    heap1.decrease_val(address_map[44], 2);
    /* Update address_map to sync with state of heap */
    address_map.insert({2, address_map[44]});
    address_map.erase(44);
    assert(heap1.get_address(2) == address_map[2]);
    assert(heap1.get_value(address_map[2]) == 2);

    /* Change the value 2 back to the value 44. */
    heap1.change_val(&address_map[2], 44);
    /* Update address_map to sync with state of heap */
    address_map.insert({44, address_map[2]});
    address_map.erase(2);
    assert(heap1.get_address(44) == address_map[44]);
    assert(heap1.get_value(address_map[44]) == 44);

    /* Remove nodes containing 3 and 8 from the heap */
    heap1.delete_elem(address_map[3]);
    heap1.delete_elem(address_map[8]);
    /* Update address_map to sync with state of heap */
    address_map.erase(3);
    address_map.erase(8);

    /* Add all addresses of elements of heap2 into address_map */
    for (int i = 1; i <= 10; i++) {
        address_map.insert({i, heap2.get_address(i)});
    }

    /* 
     * Merge contents of heap2 into heap1. This will result 
     * in heap2 becoming empty and heap1 storing the values 
     * and addresses that were once in heap2.
     */
    heap1.merge(heap2);
    assert(heap2.isEmpty());
    assert(heap1.size() == 14);
    assert(heap1.get_address(4) == address_map[4]);
    assert(heap1.get_value(address_map[4]) == 4);

    /* Clears contents of heap1 */
    heap1.clear();
    assert(heap1.isEmpty());
    /* Update address_map to sync with state of heap */
    address_map.clear();
}


