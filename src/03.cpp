#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/geometry/BoundingBox.h>
#include <lanelet2_core/geometry/Lanelet.h>
#include <lanelet2_core/geometry/Point.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/primitives/BasicRegulatoryElements.h>

using namespace lanelet;

void part1AboutLaneletMaps();
void part3QueryingInformation();

int main() {
  part1AboutLaneletMaps();
  part3QueryingInformation();
  return 0;
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
  LineString3d left = getLineStringX(0);
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

void part1AboutLaneletMaps() {
  LaneletMap map = getLaneletMap();
  PointLayer &points = map.pointLayer;
  LineStringLayer &linestrings = map.lineStringLayer;
}

void part3QueryingInformation() {
  LaneletMap laneletMap = getLaneletMap();
  Lanelet mapLanelet = *laneletMap.laneletLayer.begin();
  TrafficLight::Ptr trafficLight =
      mapLanelet.regulatoryElementsAs<TrafficLight>().front();
  auto laneletOwingLineString =
      laneletMap.laneletLayer.findUsages(mapLanelet.leftBound());
  assert(laneletOwingLineString.size() == 1 &&
         laneletOwingLineString.front() == mapLanelet);
  auto regelemsOwingLs = laneletMap.regulatoryElementLayer.findUsages(
      *trafficLight->trafficLights().front().lineString());
  assert(!regelemsOwingLs.size() == 1 &&
         regelemsOwingLs.front() == trafficLight);
  // we can find primitives with geometriacl queris. Becuase internally all
  // primitives are stored as bounding boxes, these queries only return the
  // primitives with respect to their bounding box
  Lanelets lanelets = laneletMap.laneletLayer.nearest(
      BasicPoint2d(0, 0), 1 /*the number of nn-search query*/);
  assert(!lanelets.empty());

  // to get the actually closest lanelets use this utility function
  std::vector<std::pair<double, Lanelet>> actuallyNearestLanelets =
      geometry::findNearest(laneletMap.laneletLayer, BasicPoint2d(0, 0), 1);
  assert(!actuallyNearestLanelets.empty());

  // finally we can get primitives using a search region (this also runs on the
  // bounding boxes) this returns all lanelets whose bounding box intersects
  // with the query
  Lanelets inRegion = laneletMap.laneletLayer.search(
      BoundingBox2d(BasicPoint2d(0, 0), BasicPoint2d(10, 10)));
  assert(!inRegion.empty());

  // get the first lanelet whose distance from the query is 3> m
  BasicPoint2d searchPoint = BasicPoint2d(10, 10);
  auto searchFunc = [&searchPoint](const BoundingBox2d &lltBox,
                                   const Lanelet &) {
    return geometry::distance(searchPoint, lltBox) > 3;
  };
  Optional<Lanelet> lanelet =
      laneletMap.laneletLayer.nearestUntil(searchPoint, searchFunc);
  assert(!!lanelet && geometry::distance(geometry::boundingBox2d(*lanelet),
                                         searchPoint) > 3);
}
