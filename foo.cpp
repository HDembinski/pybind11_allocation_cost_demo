#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <memory>

namespace py = pybind11;

inline std::unique_ptr<double> optional_value(bool on) {
  if (on)
    return std::make_unique<double>(1000);
  return std::unique_ptr<double>();
}

PYBIND11_MODULE(foo, m) {
    m.def("value", [](bool on) -> py::object {
      if (on)
        return py::cast(10);
      return std::move(py::none());
    });

    m.def("unique", [](bool on) -> py::object {
      auto p = optional_value(on);
      if (p)
        return py::cast(*p);
      else
        return std::move(py::none());
    });

    m.def("vector", [](int n){
      auto x = std::vector<double>(n);
      for (int i = 0; i < n; ++i)
        x.data()[i] = i;
      return x; // converts into Python list
    });

    m.def("list", [](int n){
      auto l = py::list(n);
      for (int i = 0; i < n; ++i)
        l[i] = i;
      return l;
    });

    m.def("list_fast", [](int n){
      auto l = py::list(n);
      for (int i = 0; i < n; ++i)
        PyList_SET_ITEM(l.ptr(), (ssize_t) i, py::cast(i).release().ptr());
      return l;
    });

    m.def("list_append", [](int n){
      auto l = py::list();
      for (int i = 0; i < n; ++i)
        l.append(i);
      return l;
    });

    m.def("tuple", [](int n){
      auto t = py::tuple(n);
      for (int i = 0; i < n; ++i)
        t[i] = i;
      return t;
    });

    m.def("tuple_fast", [](int n){
      auto t = py::tuple(n);
      for (int i = 0; i < n; ++i)
        PyTuple_SET_ITEM(t.ptr(), (ssize_t) i, py::cast(i).release().ptr());
      return t;
    });

    m.def("tuple_from_list", [](int n){
      auto l = py::list();
      for (int i = 0; i < n; ++i)
        l.append(i);
      return py::tuple(l);
    });

    m.def("numpy", [](int n){
      auto x = py::array_t<double>(n);
      auto d = x.mutable_data();
      for (int i = 0; i < n; ++i)
        d[i] = i;
      return x;
    });
}
