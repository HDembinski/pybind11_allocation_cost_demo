# pybind11 experiments: comparing costs of returning values and arrays of values using different approaches

This demo shows the costs for returning values and arrays of values. A main theme is the cost of memory allocation in C++, which is not negligible. Allocating temporary C++ objects should be avoided if possible.

In C++, allocation from the Heap (with new or malloc) should be done sparingly in performance critical code, because it can incur a large cost, O(100) or much more CPU cycles. The general recommendation is to avoid allocating from the Heap and allocate from the Stack instead when possible, which is O(1) CPU cycles.

But as always, things are not so simple, because system engineers know about these costs and use various methods to mitigate them, especially for code that people write commonly. Doing something clever may not produce faster code, because the naive approach is anticipated and optimized, while the compiler may not be able to optimize "clever" code.

I wrote a simple pybind11 module that returns values and arrays of values in different ways. We compare the speed of execution of the Python functions with timeit.

# Results

Take a quick look into the code how the different functions are implemented, I won't explain it here. The code was compiled with `-O3`.

```sh
$ python3 -m timeit -s "import foo" "foo.value(0)"
1000000 loops, best of 5: 347 nsec per loop
$ python3 -m timeit -s "import foo" "foo.value(1)"
1000000 loops, best of 5: 393 nsec per loop
$ python3 -m timeit -s "import foo" "foo.unique(0)"
1000000 loops, best of 5: 357 nsec per loop
$ python3 -m timeit -s "import foo" "foo.unique(1)"
1000000 loops, best of 5: 364 nsec per loop
```

`unique(...)` should be much slower than `value(...)`, but the performance is equal, even with `-O0`. This suggests that allocation of small PODs is optimized on the system level.

```sh
$ python3 -m timeit -s "import foo" "foo.vector(100)"
200000 loops, best of 5: 1.38 usec per loop
$ python3 -m timeit -s "import foo" "foo.list(100)"
200000 loops, best of 5: 1.38 usec per loop
$ python3 -m timeit -s "import foo" "foo.list_fast(100)"
200000 loops, best of 5: 994 nsec per loop
$ python3 -m timeit -s "import foo" "foo.tuple(100)"
200000 loops, best of 5: 1.33 usec per loop
$ python3 -m timeit -s "import foo" "foo.tuple_fast(100)"
500000 loops, best of 5: 956 nsec per loop
$ python3 -m timeit -s "import foo" "foo.list_append(100)"
100000 loops, best of 5: 2.15 usec per loop
$ python3 -m timeit -s "import foo" "foo.tuple_from_list(100)"
100000 loops, best of 5: 2.62 usec per loop
$ python3 -m timeit -s "import foo" "foo.numpy(100)"
500000 loops, best of 5: 802 nsec per loop

$ python3 -m timeit -s "import foo" "foo.vector(1000)"
20000 loops, best of 5: 15.9 usec per loop
$ python3 -m timeit -s "import foo" "foo.list(1000)"
20000 loops, best of 5: 18.5 usec per loop
$ python3 -m timeit -s "import foo" "foo.list_fast(1000)"
20000 loops, best of 5: 15.3 usec per loop
$ python3 -m timeit -s "import foo" "foo.tuple(1000)"
20000 loops, best of 5: 18.6 usec per loop
$ python3 -m timeit -s "import foo" "foo.tuple_fast(1000)"
20000 loops, best of 5: 15.3 usec per loop
$ python3 -m timeit -s "import foo" "foo.list_append(1000)"
10000 loops, best of 5: 23.5 usec per loop
$ python3 -m timeit -s "import foo" "foo.tuple_from_list(1000)"
10000 loops, best of 5: 26.7 usec per loop
$ python3 -m timeit -s "import foo" "foo.numpy(1000)"
200000 loops, best of 5: 1.31 usec per loop
```

Filling a vector first in C++ and letting pybind11 convert it into a Python list should be slower than allocating a Python list or tuple and filling that directly, because memory is allocated once more than necessary for the vector. For small vectors, this is correct, but not for large vectors (N=1000+). For sufficiently large vectors the allocation cost for the vector is negligible compared to the cost of filling the list or tuple, which is very costly in Python.

Allocating and filling lists or tuples make no difference in speed. However, using the unchecked `PyList_SET_ITEM` and `PyTuple_SET_ITEM` rather than the obvious pybind11 API makes a noticable difference. These unchecked functions are also used internally by pybind11 when it converts the vector to a list automatically.

For a list it is better to allocate first and then set the items instead of filling with `append`, no surprise here.

Directly filling a tuple is better than first filling a list and then to convert it to tuple, also no surprise here.

Allocating and filling a numpy array is the fastest, of course, since there is no additional raw type to Python object conversion needed for each item in the array.
