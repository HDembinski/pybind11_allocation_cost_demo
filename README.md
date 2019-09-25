# pybind11_allocation_cost_demo

This demo shows the memory allocation costs for values and arrays of values.

In C++, allocation from the Heap (with new or malloc) is generally done sparsely, because it can be huge cost, O(100) or much more CPU cycles. So the general recommendation is to avoid allocating from the Heap and allocate from the Stack instead, which is O(1) CPU cycles.

But as always, things are not so simple, because system engineers know about these costs and use various methods to mitigate them, especially for code that people write commonly.

I wrote a simple pybind11 module that returns values and arrays of values in different ways. We compare the speed of execution of the Python functions with timeit.

# Benchmark

See into code how the different functions are implemented. The code was compiled with `-O3`.

```sh
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "value(0)"
1000000 loops, best of 5: 316 nsec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "value(1)"
1000000 loops, best of 5: 309 nsec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "unique(1)"
1000000 loops, best of 5: 311 nsec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "unique(0)"
1000000 loops, best of 5: 314 nsec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "vector(100)"
200000 loops, best of 5: 1.36 usec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "tuple(100)"
200000 loops, best of 5: 1.31 usec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "tuple_from_list(100)"
100000 loops, best of 5: 2.51 usec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "numpy(100)"
500000 loops, best of 5: 774 nsec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "vector(1000)"
20000 loops, best of 5: 15.8 usec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "tuple(1000)"
20000 loops, best of 5: 19.1 usec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "tuple_from_list(1000)"
10000 loops, best of 5: 26.7 usec per loop
$ python3 -m timeit -s "from foo import value, unique, vector, numpy, tuple, tuple_from_list" "numpy(1000)"
200000 loops, best of 5: 1.24 usec per loop
```

# Conclusions

* `unique` should be much slower than `value`, but the performance is equal. Even with `-O0`, both are similar. This suggests that allocation of small PODs is optimized somewhere.
* Filling a vector first in C++ and let pybind11 convert it into a Python list should be slower than allocating a Python tuple and filling that directly, because memory is allocated twice, first for the vector and then for the list. For small vectors, this seems to be correct, but for large vectors, it is not. Python optimizes the allocation of small tuples a lot, but not very large tuples.
* Allocating and filling a numpy array is faster of course, than the other solutions which don't use numpy.