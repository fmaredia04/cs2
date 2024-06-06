#ifndef POINT_H
#define POINT_H

#include <iostream>

enum Direction {
  DEFAULT = -1,
  _NORTH = 0,
  _SOUTH = 1,
  _WEST = 2,
  _EAST = 3,
  UP = 4,
  DOWN = 5
};

struct Point {
  int x;
  int y;
  int z;

  Point() {}
  Point(int x, int y, int z): x(x), y(y), z(z) {}

  Point inc(const Direction &dir) const;

};



std::istream& operator >> (std::istream& stream, Point& point);
std::ostream& operator << (std::ostream& stream, const Point& point);

#endif
