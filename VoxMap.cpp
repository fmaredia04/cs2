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

VoxMap::VoxMap(std::istream& stream) {
// (x, y, z)
  std::string line;
  getline(stream, line, ' ');
  bounds.x = stoi(line);
  getline(stream, line, ' ');
  bounds.y = stoi(line);
  getline(stream, line);
  bounds.z = stoi(line);

  map = new Voxel**[bounds.z];
  for (int i = 0; i < bounds.z; i++) {
    map[i] = new Voxel*[bounds.y];
    for (int j = 0; j < bounds.y; j++)
        map[i][j] = new Voxel[bounds.x];
  }

  int height = -1;
  int ns = -1; // tells what line is being read (North-South coordinate)
  while (std::getline(stream, line)) {
    if (line.size() == 0) {
      height++;
      ns = -1;
      continue;
    }

    ns++;
    int end = line.size();
    for (int i = 0; i < end; i++) {
      
      // giant switch case to save on runtime
      switch (line[i]) {
        case 'f': // 1111
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
          break;
        case 'e': // 1110
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          set_air(height, ns, 4 * i + 3);
        break;
        case 'd': // 1101
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          set_air(height, ns, 4 * i + 2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
        break;
        case 'c': // 1100
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          set_air(height, ns, 4 * i + 2);
          set_air(height, ns, 4 * i + 3);
        break;
        case 'b': // 1011
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          set_air(height, ns, 4 * i + 1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
        break;
        case 'a': // 1010
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          set_air(height, ns, 4 * i + 1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          set_air(height, ns, 4 * i + 3);
        break;
        case '9': // 1001
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          set_air(height, ns, 4 * i + 1);
          set_air(height, ns, 4 * i + 2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
        break;
        case '8': // 1000
          map[height][ns][4 * i] = Voxel(-1,height,ns,4*i);
          set_air(height, ns, 4 * i + 1);
          set_air(height, ns, 4 * i + 2);
          set_air(height, ns, 4 * i + 3);
        break;
        case '7': // 0111
          set_air(height, ns, 4 * i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
        break;
        case '6': // 0110
          set_air(height, ns, 4 * i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          set_air(height, ns, 4 * i + 3);
        break;
        case '5': // 0101
          set_air(height, ns, 4 * i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          set_air(height, ns, 4 * i + 2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
        break;
        case '4': // 0100
          set_air(height, ns, 4 * i);
          map[height][ns][4 * i + 1] = Voxel(-1,height,ns,4*i+1);
          set_air(height, ns, 4 * i + 2);
          set_air(height, ns, 4 * i + 3);
        break;
        case '3': // 0011
          set_air(height, ns, 4 * i);
          set_air(height, ns, 4 * i + 1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
        break;
        case '2': // 0010
          set_air(height, ns, 4 * i);
          set_air(height, ns, 4 * i + 1);
          map[height][ns][4 * i + 2] = Voxel(-1,height,ns,4*i+2);
          set_air(height, ns, 4 * i + 3);
        break;
        case '1': // 0001
          set_air(height, ns, 4 * i);
          set_air(height, ns, 4 * i + 1);
          set_air(height, ns, 4 * i + 2);
          map[height][ns][4 * i + 3] = Voxel(-1,height,ns,4*i+3);
        break;
        case '0': // 0000
          set_air(height, ns, 4 * i);
          set_air(height, ns, 4 * i + 1);
          set_air(height, ns, 4 * i + 2);
          set_air(height, ns, 4 * i + 3);
        break;
        default:
          throw std::runtime_error("Non standard Voxel");
        break;
      }
    }
  }

//   fstream log;
//   log.open("./path.log", ios::out | ios::trunc);

//    for(int i = 0; i < bounds.z; i++){
//      for(int j = 0; j < bounds.y; j++){
//        for(int k = 0; k < bounds.x; k++){
//         if (map[i][j][k].fall == -1){
//           log << map[i][j][k].self.x << " " << map[i][j][k].self.y << " " << map[i][j][k].self.z << endl;
//        }
//     }
//    }
//    }
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

void VoxMap::mark(Voxel& at, const Direction &from, const Tracker &new_state, queue<Voxel> &q) {
  at.state = new_state;
  at.dir = from;
  if (at.fall == 0) q.push(at);
}

std::string VoxMap::dir_to_log(const Direction &dir) {
  switch (dir) {
    case _NORTH:
      return " magenta_glazed_terracotta[facing=north]";
    case _SOUTH:
      return " magenta_glazed_terracotta[facing=south]";
    case _EAST:
      return " magenta_glazed_terracotta[facing=east]";
    case _WEST:
      return " magenta_glazed_terracotta[facing=west]";
    case DOWN:
      return " redstone_block";
    default:
      return "";
  }
  return "";
}

std::string VoxMap::vxl_to_log(const Voxel &v) {
  std::string builder = "";
  builder += to_string(v.self.x);
  builder += " ";
  builder += to_string(v.self.y);
  builder += " ";
  builder += to_string(v.self.z);
  return builder;
}

Route VoxMap::route(Point src, Point dst) {
  if(!isValid(src) || map[src.z][src.y][src.x].fall > 0 || map[src.z][src.y][src.x].fall < 0 ){
    throw InvalidPoint(src);
  }
  if(!isValid(dst) || map[dst.z][dst.y][dst.x].fall > 0 || map[dst.z][dst.y][dst.x].fall < 0 ){
    throw InvalidPoint(dst);
  }

  queue<Voxel*> source;
  queue<Voxel*> target;
  bool found = false;

  remove("./path.log");

  fstream log;
  log.open("./path.log", ios::out);

  Voxel* start = &at(src);
  start->state = SOURCE;
  source.push(start);

  Voxel* end = &at(dst);
  end->state = TARGET;
  target.push(end);


  forward_list<Voxel*> visited;
  visited.push_front(start);
  visited.push_front(end);

  while (!found) {
    if (source.empty()) throw NoRoute(src, dst);
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
        curr->dir = d;
        visited.push_front(curr);
        curr = &at({curr->self.x, curr->self.y, curr->self.z - curr->fall});
        d = DOWN;
      }

      //updated curr repeat checking
      if (curr->state == SOURCE) continue;

      // We are in a target both
      if (curr->state == TARGET) {
        curr->dir = d;
        source.push(curr);
        found = true;
        break;
      }

      //log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;

      //Add to queue
      curr->state = SOURCE;
      curr->dir = d;
      source.push(curr);
      visited.push_front(curr);
    }

  }
  
  forward_list<Move> route;
  Voxel* curr = source.back();
  

  while (curr->dir != DEFAULT) {
  
   // bool fell = false;
    if (curr->dir == DOWN) {
      //fell = true;
      for (curr = curr; curr->dir < 0 || curr->dir > 3; curr = &at(curr->self.inc(UP)));
    }


    //Decide a direction, push to route, increment Voxel
    switch (curr->dir) {
      case _NORTH:
        route.push_front(NORTH);
        curr = &at(curr->self.inc(_SOUTH));
        log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;
        if (curr->fall >= 1) curr = &at(curr->self.inc(DOWN));
        break;
      case _SOUTH:
        route.push_front(SOUTH);
        curr = &at(curr->self.inc(_NORTH));
        log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;
        if (curr->fall >= 1) curr = &at(curr->self.inc(DOWN));
        break;
      case _EAST:
        route.push_front(EAST);
        //log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;
        curr = &at(curr->self.inc(_WEST));
        log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;
        if (curr->fall >= 1) curr = &at(curr->self.inc(DOWN));
        break;
      case _WEST:
        route.push_front(WEST);
        //log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;
        curr = &at(curr->self.inc(_EAST));
        log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;
        if (curr->fall >= 1) curr = &at(curr->self.inc(DOWN));
        break;
      default:
        cout << curr->self.x << " " << curr->self.y << " " << curr->self.z << " " << curr->dir << endl;
        throw runtime_error("Something bad happened");
    }
  }


  while (!visited.empty()) {
    Voxel* clear = visited.front();
    visited.pop_front();

    clear->state = UNSEEN;
    clear->dir = DEFAULT;
  }
 
  log.close();

  return Route(route.begin(), route.end());
}



