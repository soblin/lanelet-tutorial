#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_core/primitives/BasicRegulatoryElements.h>
#include <lanelet2_core/primitives/Lanelet.h>

using namespace lanelet;

void part1BasicRegulatoryElements();
void part3AddingNewRegulatoryElements();

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
  part1BasicRegulatoryElements();
  part3AddingNewRegulatoryElements();
  return 0;
}

void part1BasicRegulatoryElements() {
  LineString3d trafficLight = getLineStringY(1);
  trafficLight.attributes()[AttributeName::Type] =
      AttributeValueString::TrafficLight;
  RegulatoryElementPtr trafficLightRegelem =
      lanelet::TrafficLight::make(utils::getId(), {}, {trafficLight});
  Lanelet lanelet = getLanelet();
  lanelet.addRegulatoryElement(trafficLightRegelem);
  assert(lanelet.regulatoryElements().size() == 1);

  RegulatoryElementPtr regelem = lanelet.regulatoryElements()[0];

  assert(lanelet.regulatoryElementAs<SpeedLimit>()
             .empty()); // can collect each RegulatoryElement type
  std::vector<TrafficLight::Ptr> trafficLightRegelems =
      lanelet.regulatoryElementsAs<TrafficLight>();
  assert(trafficLightRegelems.size() == 1);
  // one trafficLightRegelem contains several associated traffic lights, like a
  // group of traffic lights that change at the same time.
  TrafficLight::Ptr tlRegelem = trafficLightRegelems.front();

  assert(tlRegelem->trafficLights().front() == trafficLight);

  // also we can set associated stop line for traffic lights
  tlRegelem->setStopLine(getLineStringY(2));
  return;
}

class LightsOn : public RegulatoryElement {
public:
  static constexpr char RuleName[] = "lights_on";

  // return the line where we are supposed to stop
  ConstLineString3d fromWhere() const {
    return getParameters<ConstLineString3d>(RoleName::RefLine).front();
  }

private:
  LightsOn(Id id, LineString3d fromWhere)
      : RegulatoryElement(std::make_shared<RegulatoryElementData>(id)) {
    parameters().insert({RoleNameString::RefLine, {fromWhere}});
  }
  friend class RegisterRegulatoryElement<LightsOn>;
  explicit LightsOn(const RegulatoryElementDataPtr &data)
      : RegulatoryElement(data) {}
};

namespace {
// this does the work of registration
RegisterRegulatoryElement<LightsOn> reg;
} // namespace

void part3AddingNewRegulatoryElements() {
  LineString3d fromWhere = getLineStringX(1);
  RuleParameterMap rules{{RoleNameString::RefLine, {fromWhere}}};
  RegulatoryElementPtr regelem =
      RegulatoryElementFactory::create("lights_on", utils::getId(), rules);
  Lanelet lanelet = getLanelet();
  lanelet.addRegulatoryElement(regelem);
  assert(!lanelet.regulatoryElementsAs<LightsOn>().empty());
}
