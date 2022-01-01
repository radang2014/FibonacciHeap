# FibonacciHeap
My own interface and implementation for the Fibonacci Heap data structure.

Useful structure for storing information in a way that allows for the following runtimes:
    BUILD EMPTY HEAP - O(1)
    BUILD HEAP FROM ARRAY - O(n)
    RETRIEVE MIN ELEMENT - O(1)
    REMOVE MIN ELEMENT - O(log n) amortized
    INSERT ELEMENT - O(1)
    DECREASE VALUE - O(1) amortized
    INCREASE VALUE - O(log n) amortized
    DELETE VALUE - O(log n) amortized
    GET VALUE FROM ADDRESS - O(1)
    GET ADDRESS FROM VALUE - O(n)
