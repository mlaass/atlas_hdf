#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "shapefile.hpp"
#include <functional>
#include <iostream>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

// Wrap 2D C++ array (given as pointer) to a numpy object.
template <typename T>
static py::array_t<T> wrap2D(T *data, size_t h, size_t w) {

  auto shape = {h, w};
  auto strides = std::vector<size_t>({sizeof(T) * w, sizeof(T)});
  auto caps = py::capsule(
      data, [](void *v) { /*delete reinterpret_cast<double *>(v);*/ });

  return py::array_t<T, py::array::forcecast | py::array::c_style>(
      shape, strides, data);
}

template <typename T> static py::array_t<T> wrap1D(T *data, size_t s) {

  auto shape = {s};
  auto strides = std::vector<size_t>({sizeof(T)});

  return py::array_t<T, py::array::forcecast | py::array::c_style>(
      shape, strides, data);
}

// This template is a handy tool to call a function f(i,j,value) for each entry
// of a 2D matrix self. template<typename func> static void
void map_matrix(const py::array_t<double> &self,
                std::function<void(int, int, double)> f) {
  if (self.ndim() != 2)
    throw(std::runtime_error("2D array expected"));
  auto s1 = self.strides(0);
  auto s2 = self.strides(1);
  const char *data = reinterpret_cast<const char *>(self.data());

  for (int i1 = 0; i1 < self.shape(0); i1++) {
    for (int i2 = 0; i2 < self.shape(1); i2++) {
      size_t offset = i1 * s1 + i2 * s2;
      // std::cout <<"("<< offset<<", "<<i1 <<", "<<i2 <<"), ";
      const double *d = reinterpret_cast<const double *>(data + offset);
      f(i1, i2, *d);
    }
  }
  std::cout << std::endl;
}
template <typename T>
void map_pointcloud(const py::array_t<T> &self,
                    std::function<void(int, int)> f) {
  if (self.ndim() != 2)
    throw(std::runtime_error("2D array expected"));
  auto s1 = self.strides(0);
  auto s2 = self.strides(1);
  const char *data = reinterpret_cast<const char *>(self.data());

  for (int i1 = 0; i1 < self.shape(0); i1++) {
    size_t offset0 = i1 * s1;
    size_t offset1 = i1 * s1 + s2;
    // std::cout <<"("<< offset<<", "<<i1 <<", "<<i2 <<"), ";
    const double *d0 = reinterpret_cast<const double *>(data + offset0);
    const double *d1 = reinterpret_cast<const double *>(data + offset1);
    f((int)*d0, (int)*d1);
  }
  std::cout << std::endl;
}

// The module begins
PYBIND11_MODULE(globimap, m) {
  m.def(
      "read_shapefile", +[](const std::string &filename) {
        std::vector<uint32_t> idx;
        std::vector<float> coords;
        size_t start = 0;
        importSHP(filename, [&](SHPObject *shp, size_t s, size_t e) {
          if (shp->nSHPType == SHPT_POLYGON) {
            idx.push_back(start + s);
            size_t d = e - s;
            idx.push_back(d);
            start += d;
            for (auto i = s; i < e; ++i) {
              coords.push_back(shp->padfX[i]);
              coords.push_back(shp->padfY[i]);
            }
          }
        });
        return std::make_tuple(wrap2D<uint32_t>(&idx[0], idx.size() / 2, 2),
                               wrap2D<float>(&coords[0], coords.size() / 2, 2));
      });
}
