#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <memory>

namespace py = pybind11;

PYBIND11_MODULE(foo, m) {
    m.def("value", [](){
      return 10.;
    });

    m.def("unique", [](){
      auto i = std::make_unique<double>(1000);
      return *i;
    });

    m.def("vector", [](){
      auto x = std::vector<double>(1000);
      for (int i = 0; i < 1000; ++i)
        x.data()[i] = i;
      return x; // converts into Python list
    });

    m.def("tuple", [](){
      auto t = py::tuple(1000);
      for (int i = 0; i < 1000; ++i)
        t[i] = i;
      return t;
    });

    m.def("tuple_from_list", [](){
      auto l = py::list();
      for (int i = 0; i < 1000; ++i)
        l.append(i);
      return py::tuple(l);
    });

    m.def("numpy", [](){
      auto x = py::array_t<double>(1000);
      double* d = x.mutable_data();
      for (int i = 0; i < 1000; ++i)
        d[i] = i;
      return x;
    });
}
