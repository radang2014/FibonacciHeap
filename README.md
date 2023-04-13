# FibonacciHeap
My own interface and implementation for the Fibonacci Heap data structure.

Useful structure for storing information in a way that allows for the following runtimes:
 * BUILD EMPTY HEAP - O(1)
 * BUILD HEAP FROM ARRAY - O(n)
 * RETRIEVE MIN ELEMENT - O(1)
 * REMOVE MIN ELEMENT - O(log n) amortized
 * INSERT ELEMENT - O(1)
 * DECREASE VALUE - O(1) amortized
 * INCREASE VALUE - O(log n) amortized
 * DELETE VALUE - O(log n) amortized
 * GET VALUE FROM ADDRESS - O(1)
 * GET ADDRESS FROM VALUE - O(n)

FILES:
* `fib-heap.h`: Interface for Fibonacci heap
* `fib-heap.cpp`: Implementation of Fibonacci heap
* `use-heap-example.cpp`: Example of how to use Fibonacci heap
* `README.md`: This file

COMPILE / RUN INSTRUCTIONS:
* To compile the example code with `g++`, type: `g++ -o use-heap-example -Wall -Wextra fib-heap.cpp use-heap-example.cpp`
* To run the example code, type: `./use-heap-example` in the directory containing the compiled executable.
* There should be no output.
