#ifndef VOXMAP_H
#define VOXMAP_H

#include <istream>
#include <vector>
#include <queue>

#include "Point.h"
#include "Route.h"

enum Tracker {
  UNSEEN = 0,
  SOURCE = 1,
  TARGET = 2
};

class VoxMap {
  // Member Variables
  struct Voxel {
    Tracker state;
    Direction dir;
    Point self;
    int fall;

    public:
      Voxel(int f){
        fall = f;
        state = UNSEEN;
        dir = DEFAULT;
      }
      Voxel(int f, int z, int y, int x){
        fall = f;
        state = UNSEEN;
        self.x = x;
        self.y = y;
        self.z = z;
        dir = DEFAULT;
      }
      Voxel(){
        fall = -1;
        state = UNSEEN;
        dir = DEFAULT;
      }
  };

  Point bounds;
  Voxel*** map;

  // Helper Functions
  bool isValid(Point p) const;
  bool isValid(int z, int y, int x) const;
  bool validSource(Voxel p) const;
  void set_air(int x, int y, int z);
  Voxel& operator[] (Point p);
  Voxel& at(int x, int y, int z);
  Voxel& at(Point p);
  void mark(Voxel& at, const Direction &from, const Tracker &new_state, std::queue<Voxel> &q);


public:
  VoxMap(std::istream& stream);
  ~VoxMap();

  Route route(Point src, Point dst);
};

#endif