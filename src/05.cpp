#include <ament_index_cpp/get_package_share_directory.hpp>
#include <lanelet2_extension/projection/mgrs_projector.hpp>
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_io/Io.h>
#include <lanelet2_routing/Route.h>
#include <lanelet2_routing/RoutingGraph.h>
#include <lanelet2_routing/RoutingGraphContainer.h>
#include <lanelet2_routing/RoutingCost.h>
#include <lanelet2_traffic_rules/TrafficRulesFactory.h>

#include <iostream>
#include <set>
#include <vector>

using namespace lanelet;
using namespace std;

void part1CreatingAndUsingRoutingGraphs(const LaneletMapPtr map);
void part2UsingRoutes(const LaneletMapPtr map);
void part2_1(const LaneletMapPtr map);
// void part3UsingRoutingGraphContainers(const LaneletMapPtr map);

int main() {
  // How to read lanelet2.osm
  string path =
      ament_index_cpp::get_package_share_directory("lanelet_tutorial");
  string fpath = path + "/mapping_example.osm";
  lanelet::ErrorMessages errors{};
  lanelet::projection::MGRSProjector projector{};
  lanelet::LaneletMapPtr map = lanelet::load(fpath, projector, &errors);
  for (auto &&error : errors)
    cout << error << endl;
  part1CreatingAndUsingRoutingGraphs(map);
  part2UsingRoutes(map);
  fpath = path + "/kashiwanoha_intersection_area.osm";
  lanelet::ErrorMessages errors2{};
  lanelet::projection::MGRSProjector projector2{};
  lanelet::LaneletMapPtr map2 = lanelet::load(fpath, projector2, &errors2);
  for (auto &&error : errors2)
    cout << error << endl;
  part2_1(map2);
  // part3UsingRoutingGraphContainers(map);
}

void part1CreatingAndUsingRoutingGraphs(const LaneletMapPtr map) {
  traffic_rules::TrafficRulesPtr trafficRules =
      traffic_rules::TrafficRulesFactory::create(Locations::Germany,
                                                 Participants::Vehicle);
  // routing graph varies depending on traffic rules
  routing::RoutingGraphPtr routingGraph =
      routing::RoutingGraph::build(*map, *trafficRules);
  ConstLanelet lanelet = map->laneletLayer.get(4984315);
  assert(!routingGraph->adjacentLeft(lanelet));
  assert(!routingGraph->adjacentRight(
      lanelet)); // adjacent lanelets are neighbours but not reachable by lane
                 // change
  cout << "\"right\" of lanelet are lane-changable one" << endl;
  cout << "routingGraph->right(lanelet) = "
       << routingGraph->right(lanelet)->id() << endl;

  cout << "routingGraph->following(lanelet).front = "
       << routingGraph->following(lanelet).front().id() << endl;

  cout << "\"besides\" return the slice of the road where the lanelet "
          "is: ";
  for (auto &&beside : routingGraph->besides(lanelet))
    cout << beside.id() << " ";
  cout << endl;

  // get all possible paths from here whose length is at least 100m
  /// 'false' flag exculdes lane-changing
  routing::LaneletPaths paths =
      routingGraph->possiblePaths(lanelet, 100, 0, false);
  assert(paths.size() == 1);
  // なんかVMBで見えるidと若干のずれがある
  cout << "path from 4984315: ";
  for (auto &&path : paths[0])
    cout << path.id() << " ";
  cout << endl;

  /// if lane-change is 'true' we have more options
  paths = routingGraph->possiblePaths(lanelet, 100, 0, true);
  cout << "if lane-change is allowed in the flag, there  are " << paths.size()
       << " paths" << endl;

  // reachable set is basically the same as possible paths, but the lanelets are
  // in an unsorted order and contain no duplicates. Also, possiblePaths
  // discards paths that are below the cost threshold while reachable set keeps
  // them all
  ConstLanelets reachableSet = routingGraph->reachableSet(lanelet, 100, 0);
  cout << "there are " << reachableSet.size() << " reachable element" << endl;

  // obtain shortest path
  ConstLanelet toLanelet = map->laneletLayer.get(2925017);
  Optional<routing::LaneletPath> shortestPath =
      routingGraph->shortestPath(lanelet, toLanelet, 1);
  assert(!!shortestPath);
  /// the shortest path can contain sudden lane changes. You can query the path
  /// for the sequence or lanelets that you can follow until you must make a
  /// lane change
  LaneletSequence lane = shortestPath->getRemainingLane(shortestPath->begin());
  for (auto &&way : lane)
    cout << way.id() << " ";
  cout << endl;
}

void part2UsingRoutes(const LaneletMapPtr map) {
  traffic_rules::TrafficRulesPtr trafficRules =
      traffic_rules::TrafficRulesFactory::create(Locations::Germany,
                                                 Participants::Vehicle);
  routing::RoutingGraphUPtr routingGraph =
      routing::RoutingGraph::build(*map, *trafficRules);
  ConstLanelet lanelet = map->laneletLayer.get(4984315);
  ConstLanelet toLanelet = map->laneletLayer.get(2925017);

  Optional<routing::Route> route =
      routingGraph->getRoute(lanelet, toLanelet, 0);

  routing::LaneletPath shortestPath = route->shortestPath();

  LaneletSequence fullLane = route->fullLane(lanelet);
  for (auto &&lane : fullLane)
    cout << lane.id() << " ";
  cout << endl;
}

void part2_1(const LaneletMapPtr map) {
  traffic_rules::TrafficRulesPtr trafficRules =
      traffic_rules::TrafficRulesFactory::create(Locations::Germany,
                                                 Participants::Vehicle);
  routing::RoutingGraphUPtr routingGraph =
      routing::RoutingGraph::build(*map, *trafficRules);
  ConstLanelet lanelet = map->laneletLayer.get(113);
  ConstLanelet toLanelet = map->laneletLayer.get(134);
  Optional<routing::Route> route =
      routingGraph->getRoute(lanelet, toLanelet, 0);

  routing::LaneletPath shortestPath = route->shortestPath();

  LaneletSequence fullLane = route->fullLane(lanelet);
  // 113 56 124 33 131 9041 30 134, which is as expeceted on VMB
  for (auto &&lane : fullLane)
    cout << lane.id() << " ";
  cout << endl;

  toLanelet = map->laneletLayer.get(112);
  route = routingGraph->getRoute(lanelet, toLanelet, 0);
  shortestPath = route->shortestPath();
  fullLane = route->fullLane(lanelet);
  // looks like the map is not well configured.
  // 113 55 122 46 118 9061 97 9121 107 15 117 54 112
  for (auto &&lane : fullLane)
    cout << lane.id() << " ";
  cout << endl;
}

// void part3UsingRoutingGraphContainers(const LaneletMapPtr map);
