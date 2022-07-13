#include <ament_index_cpp/get_package_share_directory.hpp>
#include <lanelet2_extension/projection/mgrs_projector.hpp>
#include <lanelet2_core/LaneletMap.h>
#include <lanelet2_io/Io.h>

#include <iostream>
#include <set>
#include <vector>

using namespace std;

int main() {
  // How to read lanelet2.osm
  string path =
      ament_index_cpp::get_package_share_directory("lanelet_tutorial");
  path += "/kashiwanoha_intersection_area.osm";
  lanelet::ErrorMessages errors{};
  lanelet::projection::MGRSProjector projector{};
  lanelet::LaneletMapPtr map = lanelet::load(path, projector, &errors);
  for (auto &&error : errors)
    cout << error << endl;

  // How to query point/linestring/area by id()
  lanelet::PointLayer &points = map->pointLayer;
  lanelet::LineStringLayer &linestrings = map->lineStringLayer;

  {
    lanelet::ConstLanelet l59 = map->laneletLayer.get(59);
    lanelet::ConstLineString3d l59left = l59.leftBound();
    lanelet::ConstLineString3d l59right = l59.rightBound();
    cout << "point id of l59" << endl;
    for (auto &&point : l59left)
      cout << point.id() << endl;
  }

  // How to access to/iterate over the element of each object
  {
    vector<lanelet::ConstLanelet> curves;
    for (auto &&id : {59, 51, 57, 53}) {
      curves.push_back(map->laneletLayer.get(id));
    }

    vector<lanelet::ConstLineString3d> outers;
    for (auto &&id : {59, 51, 57, 53})
      outers.push_back(map->laneletLayer.get(id).leftBound());

    vector<lanelet::ConstPoint3d> outer_points;
    for (auto &&outer : outers)
      for (auto &&point : outer)
        outer_points.push_back(point);

    set<int> outer_point_ids;
    for (auto &&point : outer_points)
      outer_point_ids.insert(point.id());

    lanelet::ConstPolygon3d intersection_area = map->polygonLayer.get(10000);
    cout << "point id of intersection_area_10000" << endl;
    for (auto &&point : intersection_area)
      cout << point.id() << endl;
  }

  // The coordinate of each point is defined
  {
    lanelet::ConstPoint3d point1886 = points.get(1886);
    cout << "x = " << point1886.x() << ", y = " << point1886.y()
         << ", z = " << point1886.z() << endl;
  }

  // How to get a route
}
