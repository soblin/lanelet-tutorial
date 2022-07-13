#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/primitives/BasicRegulatoryElements.h>
#include <lanelet2_core/primitives/Lanelet.h>
#include <lanelet2_core/utility/Units.h>
#include <lanelet2_traffic_rules/TrafficRules.h>
#include <lanelet2_traffic_rules/TrafficRulesFactory.h>

using namespace lanelet;

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

int main() {
  LineString3d leftLs{getLineStringY(3)};
  LineString3d middleLs{getLineStringY(2)};
  LineString3d rightLs{getLineStringY(0)};
  LineString3d nextLeftLs{
      utils::getId(),
      {middleLs.back(), Point3d(utils::getId(), middleLs.back().x() + 1.0,
                                middleLs.back().y())}};
  LineString3d nextRightLs{
      utils::getId(),
      {rightLs.back(),
       Point3d(utils::getId(), rightLs.back().x() + 1.0, rightLs.back().y())}};
  Lanelet left{utils::getId(), leftLs, middleLs};
  Lanelet right{utils::getId(), middleLs, rightLs};
  Lanelet next{utils::getId(), nextLeftLs, nextRightLs};

  traffic_rules::TrafficRulesPtr trafficRules =
      traffic_rules::TrafficRulesFactory::create(Locations::Germany,
                                                 Participants::Vehicle);
  traffic_rules::TrafficRulesPtr pedestrianRules =
      traffic_rules::TrafficRulesFactory::create(Locations::Germany,
                                                 Participants::Pedestrian);

  // default "interpretation" of lanelet
  assert(trafficRules->canPass(right));     // by default "road lanelets
  assert(!pedestrianRules->canPass(right)); // and not passable for pedestrian

  // no lane change
  assert(!trafficRules->canPass(right, left));

  // we can also query the speed limit
  traffic_rules::SpeedLimitInformation limit = trafficRules->speedLimit(right);
  assert(limit.speedLimit == 50_kmh);
  assert(
      limit.isMandatory); // mandatory means we must not exceed the speed limit

  // now lets start to add some tags to get more meaningful results
  /// to allow lane change
  middleLs.attributes()[AttributeName::Type] = AttributeValueString::LineThin;
  middleLs.attributes()[AttributeName::Subtype] = AttributeValueString::Dashed;
  right.attributes()[AttributeName::Type] = AttributeValueString::Lanelet;
  right.attributes()[AttributeName::Subtype] = AttributeValueString::Road;
  right.attributes()[AttributeName::Location] = AttributeValueString::Nonurban;
  next.attributes() = right.attributes();
  left.attributes()[AttributeName::OneWay] = false;

  // now we can see that lane change is allowed
  assert(trafficRules->canChangeLane(right, left));
  assert(trafficRules->cahChangeLane(left, right));

  // and left is no drivable in inverted direction
  assert(!trafficRules->canChangeLane(left, right.invert()));
  assert(!trafficRules->canChangeLane(left.invert(), right.invert()));

  // also the reported speed limit is different (because lanelets are now
  // nonurban)
  limit = trafficRules->speedLimit(right);
  assert(limit.speedLimit == 100_kmh); // on german nonurban roads

  // if we now add a speed limit regulatory element, the speed limit
  // changes(because what we've seen above is default speed limit of German)
  LineString3d sign = getLineStringX(3);
  SpeedLimit::Ptr speedLimit =
      SpeedLimit::make(utils::getId(), {}, {{sign}, "de274-60"});
  right.addRegulatoryElement(speedLimit);
  assert(trafficRules->speedLimit(right).speedLimit == 60_kmh);

  // if the type of the lanelet is changed from road to walkway, it is not
  // longer drivable for vehicles
  right.attributes()[AttributeName::Subtype] = AttributeValueString::Crosswalk;
  assert(!trafficRules->canPass(right));
  assert(pedestrianRules->canPass(right));
}
