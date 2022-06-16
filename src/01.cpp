#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/Area.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <lanelet2_core/primitives/Area.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_core/utility/Units.h>

#include <iostream>
#include <thread>
#include <mutex>

using namespace lanelet;

std::mutex cout_mutex;
void part0Primitives() {
  Point3d p(utils::getId(), 0, 0, 0);
  // shallow copy; sharing same data
  Point3d pCopy = p;
  assert(p == pCopy);

  p.z() = 2;
  p.setId(utils::getId());

  assert(p.id() == pCopy.id());
  assert(p.z() == pCopy.z());

  std::lock_guard<std::mutex> lg(cout_mutex);
  std::cout << "id = " << p.id() << ", x = " << p.x() << ", y = " << p.y()
            << std::endl;
}

int main() {
  std::thread t1(part0Primitives), t2(part0Primitives);
  t1.join();
  t2.join();
  return 0;
}
