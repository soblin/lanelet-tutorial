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
#include <iomanip>

using namespace lanelet;
using namespace std;

void part0Primitives() {
  cout << boolalpha;
  Point3d p(utils::getId(), 0, 0, 0);
  // shallow copy; sharing same data
  Point3d pCopy = p;
  assert(p == pCopy);

  p.z() = 2;
  p.setId(utils::getId());

  assert(p.id() == pCopy.id());
  assert(p.z() == pCopy.z());

  cout << "id = " << p.id() << ", x = " << p.x() << ", y = " << p.y() << endl;

  // attributes() is KV store
  p.attributes()["type"] = "point";
  p.attributes()["pi"] = 3.14;
  using namespace lanelet::units::literals;
  p.attributes()["velocity"] = 5_kmh;
  assert(p.attributes()["velocity"] == pCopy.attributes()["velocity"]);
  assert(pCopy.attributes()["type"] == "point");

  // default value if the key does not exist
  /// this key does not exist
  cout << "p.attributeOr(\"nonexistent\", -1) = "
       << p.attributeOr("nonexistent", -1) << endl;
  // this key cannot be converted to expected type, so fallback
  cout << "p.attributeOr(\"type\", 0) = " << p.attributeOr("type", 0) << endl;
  /// 0 and 0_kmh is different type
  cout << "p.attributeOr(\"velocity\", 0) = "
       << p.attributeOr("velocity", 0_kmh) << endl;
  cout << "p.attributeOr(\"velocity\", 0_kmh) == 5_kmh : "
       << (p.attributeOr("velocity", 0_kmh) == 5_kmh) << endl;
}

int main() {
  part0Primitives();
  return 0;
}
