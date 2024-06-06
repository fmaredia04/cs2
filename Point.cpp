#include "Point.h"

Point Point::inc(const Direction &dir) const {
  switch (dir) {
    case UP:
      return {x, y, z + 1};
      break;
    case DOWN:
      return {x, y, z - 1};
      break;
    case _NORTH:
      return {x, y - 1, z};
      break;
    case _SOUTH:
      return {x, y + 1, z};
      break;
    case _WEST:
      return {x - 1, y, z};
      break;
    case _EAST:
      return {x + 1, y, z};
      break;
    default:
      return {x, y, z};
  }
}

std::istream& operator >> (std::istream& stream, Point& point) {
  return stream >> point.x >> point.y >> point.z;
}

std::ostream& operator << (std::ostream& stream, const Point& point) {
  return stream << '(' << point.x << ", " << point.y << ", " << point.z << ')';
}
