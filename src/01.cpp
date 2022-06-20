#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/Area.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <lanelet2_core/primitives/Area.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/LineString.h>
#include <lanelet2_core/primitives/Point.h>
#include <lanelet2_core/primitives/Polygon.h>
#include <lanelet2_core/primitives/BasicRegulatoryElements.h>
#include <lanelet2_core/utility/Units.h>

#include <iostream>
#include <iomanip>
#include <cassert>

using namespace lanelet;
using namespace std;

void part0Primitives() {
  cout << "part0Primitives" << endl;
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
  cout << endl;
}

void part1Points() {
  cout << "part1Points" << endl;
  Point3d p3d(utils::getId(), 1, 2, 3);

  {
    // this is eigen point
    BasicPoint3d &p3dBasic = p3d.basicPoint();
    p3dBasic.z() = 4;
    // convenience: can twice its internal memory through eigen computation
    BasicPoint3d pTwice = p3dBasic * 2;
    p3d.basicPoint() = pTwice;
    cout << "p3d * 2: x = " << p3d.x() << ", y = " << p3d.y()
         << ", z = " << p3d.z() << endl;
  }
  {
    Point2d p2d = utils::to2D(p3d);
    cout << "p2d: x = " << p2d.x() << ", y = " << p2d.y() << endl;
  }
  cout << endl;
}

void part2LineStrings() {
  cout << "part2LineStrings" << endl;
  Point3d p1{utils::getId(), 0, 0, 0};
  Point3d p2{utils::getId(), 1, 0, 0};
  Point3d p3{utils::getId(), 2, 0, 0};
  // connect set of points with straight line
  LineString3d ls(utils::getId(), {p1, p2, p3});
  for (const auto &p : ls) {
    assert(p.z() == 0);
  }
  // similar to std::vector
  ls.push_back(Point3d(utils::getId(), 3, 0, 0));
  assert(ls.size() == 4);
  ls.pop_back();
  assert(ls.size() == 3);

  // built-in attributes for LineString
  ls.attributes()[AttributeName::Type] = AttributeValueString::LineThin;
  ls.attributes()[AttributeName::Subtype] = AttributeValueString::Dashed;

  // line segment (pair of adjacent points)
  assert(ls.numSegments() == 2);
  Segment3d segment = ls.segment(1);
  cout << "segment.first: x = " << segment.first.x()
       << ", y = " << segment.first.y() << endl;
  cout << "segment.second: x = " << segment.second.x()
       << ", y = " << segment.second.y() << endl;

  // reversed LineString: still the points are shared
  LineString3d lsInv = ls.invert();
  segment = lsInv.segment(0);
  cout << "segment.first: x = " << segment.first.x()
       << ", y = " << segment.first.y() << endl;
  cout << "segment.second: x = " << segment.second.x()
       << ", y = " << segment.second.y() << endl;

  // convert LineString3D to LineString2D
  LineString2d ls2d = utils::to2D(ls);
  Point2d front2D = ls2d.front();
  cout << "front2D: x = " << front2D.x() << ", y = " << front2D.y() << endl;

  // this is a vector of eigen points
  BasicLineString3d lsBasic = ls.basicLineString();
  for (int i = 0; i < lsBasic.size(); ++i) {
    cout << "lsBasic #" << i << ": x = " << lsBasic[i].x()
         << ", y =  " << lsBasic[i].y() << endl;
  }
  cout << endl;
}

void part3Polygons() {
  cout << "part3Polygons\n" << endl;
  // the difference between Polygon<N>d and LineString<N>d is that the last and
  // first point is also connected to close the polygon
  Point3d p1{utils::getId(), 0, 0, 0};
  Point3d p2{utils::getId(), 1, 0, 0};
  Point3d p3{utils::getId(), 2, -1, 0};
  Polygon3d poly(utils::getId(), {p1, p2, p3});

  // size() is the # of points
  // numSegment() equals to the # of points
  assert(poly.size() == 3);
  assert(poly.numSegments() == 3);
  assert(poly.segment(2).first == p3 && poly.segment(2).second == p1);
}

LineString3d getLineStringX(double x) {
  return LineString3d(utils::getId(), {Point3d(utils::getId(), x, 0, 0),
                                       Point3d(utils::getId(), x, 1, 0),
                                       Point3d(utils::getId(), x, 2, 0)});
}

LineString3d getLineStringY(double y) {
  return LineString3d(utils::getId(), {Point3d(utils::getId(), 0, y, 0),
                                       Point3d(utils::getId(), 1, y, 0),
                                       Point3d(utils::getId(), 2, y, 0)});
}

Polygon3d getPolygon() {
  Point3d p1(utils::getId(), 0, 0, 0);
  Point3d p2(utils::getId(), 2, 0, 0);
  Point3d p3(utils::getId(), 2, -2, 0);
  return Polygon3d(utils::getId(), {p1, p2, p3});
}

Area getArea() {
  LineString3d top = getLineStringY(2);
  LineString3d right = getLineStringX(2).invert();
  LineString3d bottom = getLineStringY(0).invert();
  LineString3d left = getLineStringY(0);
  return Area(utils::getId(), {top, right, bottom, left});
}

Lanelet getLanelet() {
  LineString3d left = getLineStringY(2);
  LineString3d right = getLineStringY(0);
  return Lanelet(utils::getId(), left, right);
}

RegulatoryElementPtr getRegulatoryElement() {
  LineString3d trafficLight = getLineStringX(3);
  return TrafficLight::make(utils::getId(), {}, {trafficLight});
}

LaneletMap getLaneletMap() {
  auto area = getArea();
  auto lanelet = getLanelet();
  lanelet.addRegulatoryElement(getRegulatoryElement());
  return std::move(*utils::createMap({lanelet}, {area}));
}

void part4Lanelets() {
  cout << "part4Lanelets" << endl;
  LineString3d left = getLineStringY(1);
  LineString3d right = getLineStringY(0);
  Lanelet lanelet(utils::getId(), left, right);
  assert(lanelet.leftBound() == left);
  assert(lanelet.rightBound() == right);
  lanelet.setLeftBound(left); // if need to change this later

  ConstLineString3d centerline =
      lanelet.centerline(); // automatically computes. this is constant

  // centerline is cached, so cenerline and centerline2 is pointing to same data
  ConstLineString3d centerline2 = lanelet.centerline();
  assert(centerline2 == centerline);
  // re-computes centerline automatically, so centerline changes to another
  // resource
  lanelet.setLeftBound(getLineStringY(2));
  ConstLineString3d centerline3 = lanelet.centerline();
  assert(centerline3 != centerline);

  // if the bound is directly changed, need to reset() manually
  right.push_back(Point3d(utils::getId(), 4, 0, 0));
  assert(centerline3 == lanelet.centerline()); // centerline3 is not updated
  lanelet.resetCache();
  assert(centerline3 != lanelet.centerline());
  right.pop_back();
  lanelet.resetCache();

  // inverted Lanelet
  /// Original
  // A1 -> A2 -> A3
  // B1 -> B2 -> B3
  /// Inverted
  // B3 -> B2 -> B1
  // A3 -> A2 -> A1
  Lanelet laneletInv = lanelet.invert();
  assert(laneletInv.leftBound().front == lanelet.rightBound().back()); // B3
  assert(laneletInv.rightBound.back() == lanelet.leftBound.front());   // A1
  assert(laneletInv.constData() == lanelet.constData());

  // CompondLPolygon is the geometic bound (clockwise order)
  CompoundPolygon3d polygon =
      lanelet.polygon3d(); // A1 -> A2 -> A3 -> B3 -> B2 -> B1
  assert(polygon.size == 6);
  assert(polygon[0] == lanelet.leftBound().front()); // A1
  assert(polygon.back() == lanelet.rightBound.front());
  cout << endl;
}

int main() {
  part0Primitives();
  part1Points();
  part2LineStrings();
  part3Polygons();
  part4Lanelets();
  return 0;
}
