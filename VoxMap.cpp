#include "VoxMap.h"
#include "Errors.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <string>
#include <cstdio>
#include <forward_list>
using namespace std;

bool VoxMap::isValid(Point p) const {
  return (p.x < bounds.x && p.y < bounds.y && p.z < bounds.z) && (p.x >= 0 && p.y >= 0 && p.z >= 0);
}

bool VoxMap::isValid(int z, int y, int x) const {
  return (x < bounds.x && y < bounds.y && z < bounds.z) && (x >= 0 && y >= 0 && z >= 0);
}

bool VoxMap::validSource(Voxel p) const{
  if(p.fall < 0){
    return false;
  }
  if(p.fall > 0){
    return false;
  }
  return true;
}


VoxMap::Voxel& VoxMap::operator[] (Point p) {
  Point use(p.z, p.y, p.z);
  if (!isValid(use)) throw InvalidPoint(use);
  return map[p.z][p.y][p.x];
}

VoxMap::Voxel& VoxMap::at(Point p) {
  Point use(p.z, p.y, p.z);
  return map[p.z][p.y][p.x];
}

VoxMap::Voxel& VoxMap::at (int z, int y, int x) {
  Point use(z, y, x);
  if (!isValid(use)) throw InvalidPoint(use);
  return map[z][y][x];
}

void VoxMap::set_air(int z, int y, int x) {
  if (z - 1 <= -1) {
    map[z][y][x].fall = 50000;
    map[z][y][x].self.x = x;
    map[z][y][x].self.y = y;
    map[z][y][x].self.z = z;
    return;
  }
  map[z][y][x].fall = map[z-1][y][x].fall + 1;
  map[z][y][x].self.x = x;
  map[z][y][x].self.y = y;
  map[z][y][x].self.z = z;
}

VoxMap::~VoxMap(){
  // FIXME
  for (int i = 0; i < bounds.z; i++) {
    for (int j = 0; j < bounds.y; j++){
      delete[] map[i][j];
    }
    delete[] map[i];
  }
  delete[] map; 
}

Route VoxMap::route(Point src, Point dst) {
  if(!isValid(src) || map[src.z][src.y][src.x].fall > 0 || map[src.z][src.y][src.x].fall < 0 ){
    throw InvalidPoint(src);
  }
  if(!isValid(dst) || map[dst.z][dst.y][dst.x].fall > 0 || map[dst.z][dst.y][dst.x].fall < 0 ){
    throw InvalidPoint(dst);
  }

  queue<Voxel*> source;
  bool found = false;

  Voxel* start = &at(src);
  start->state = SOURCE;
  start->parent = {src.x,src.y,src.z};
  source.push(start);

  Voxel* end = &at(dst);
  end->state = TARGET;


  forward_list<Voxel*> visited;
  visited.push_front(start);
  visited.push_front(end);

  while (!found) {
    if (source.empty()){
      while (!visited.empty()) {
      Voxel* clear = visited.front();
      visited.pop_front();

      clear->state = UNSEEN;
      clear->parent = {-1,-1,-1};
      }
     throw NoRoute(src, dst);
    }
    Voxel* parent = source.front();
    source.pop();

    for (short i = 0; i < 4; i++) {
      Direction d = Direction(i);

      // Valid move ?
      Point inc = parent->self.inc(d);
      if (!isValid(inc)) continue;

      Voxel* curr = &at(inc);
      if (curr->fall >= 50000) continue;
      if (curr->state == SOURCE) continue;
      // cout << curr->self.inc(UP).z << " " << curr->self.inc(UP).y << " " << curr->self.inc(UP).x << endl;
      // cout << "Valid: " << isValid(curr->self.inc(UP)) << endl;
      // cout << (curr->self.inc(UP).z < bounds.z) << endl;
      // cout << bounds.z << " Y: " << bounds.y << endl;
      if (isValid(curr->self.inc(UP)) && curr->fall == -1 && (at(curr->self.inc(UP)).fall == 0 && at(parent->self.inc(UP)).fall >= 1)) {
        curr = &at(curr->self.inc(UP));
      } else if (curr->fall == -1) continue;

      // if we have a fall, mark the top block and the bottom block
      if (curr->fall > 0) {
        curr->state = SOURCE;
        curr->parent = {parent->self.x,parent->self.y,parent->self.z};
        visited.push_front(curr);
        curr = &at({curr->self.x, curr->self.y, curr->self.z - curr->fall});
        d = DOWN;
      }

      //updated curr repeat checking
      if (curr->state == SOURCE) continue;

      // We are in a target both
      if (curr->state == TARGET) {
        curr->parent = {parent->self.x,parent->self.y,parent->self.z};
        source.push(curr);
        found = true;
        break;
      }

      //log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;

      //Add to queue
      curr->state = SOURCE;
      curr->parent = {parent->self.x,parent->self.y,parent->self.z};
      source.push(curr);
      visited.push_front(curr);
    }

  }
  
  forward_list<Move> route;
  Voxel* curr = source.back();
  
  // Make while loop to retrace the steps
  while (curr->self.x != curr->parent.x || curr->self.y != curr->parent.y || curr->self.z != curr->parent.z) {
    //cout << "Curr: " << curr->self.x << "," << curr->self.y << "," << curr->self.z << " Parent: " << curr->parent.x << "," << curr->parent.y << "," << curr->parent.z <<endl;

   // bool fell = false;
    // if (curr->dir == DOWN) {
    //   //fell = true;
    //   for (curr = curr; curr->dir < 0 || curr->dir > 3; curr = &at(curr->self.inc(UP)));
    // }

    if(curr->self.y < curr->parent.y){
      route.push_front(NORTH);
      curr = &at(curr->parent);
    }
    else if(curr->self.y > curr->parent.y){
      route.push_front(SOUTH);
      curr = &at(curr->parent);
    }
    else if(curr->self.x > curr->parent.x){
      route.push_front(EAST);
      curr = &at(curr->parent);
    }
    else if(curr->self.x < curr->parent.x){
      route.push_front(WEST);
      curr = &at(curr->parent);
    }
  }


  while (!visited.empty()) {
    Voxel* clear = visited.front();
    visited.pop_front();

    clear->state = UNSEEN;
    clear->parent = {-1,-1,-1};
  }
  //cout << route.empty() << endl;
  return Route(route.begin(), route.end());
}



