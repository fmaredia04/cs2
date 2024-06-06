#include "VoxMap.h"
#include "Errors.h"
#include <iostream>
#include <fstream>
#include <queue>
#include <cstdio>
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
  log << src.x << " " << src.y << " " << src.z << endl;

  Voxel* start = &at(src);
  start->state = SOURCE;
  source.push(start);

  Voxel* end = &at(dst);
  end->state = TARGET;
  target.push(end);

  while (!found) {
    if (source.empty()) throw NoRoute(src, dst);
    Voxel* parent = source.front();
    source.pop();

    for (Direction d = Direction::_NORTH; d < 4; d = Direction(d+1)) {

      // Valid move ?
      Point inc = parent->self.inc(d);
      if (!isValid(inc)) continue;
      Voxel* curr = &at(inc);
      if (curr->fall == -1 || curr->fall >= 50000) continue;

      if (curr->fall > 0) {
        curr->state = SOURCE;
        curr->dir = d;
        curr = &at({curr->self.x, curr->self.y, curr->self.z - curr->fall});
          
      }

      // Valid push / have we found a Target path
      if (curr->state == TARGET) {
        source.push(curr);
        found = true;
        cout << "found" << endl;
        break;
      }
      if (curr->state == SOURCE) continue;

      log << curr->self.x << " " << curr->self.y << " " << curr->self.z << endl;

      //Add to queue
      curr->state = SOURCE;
      curr->dir = d;
      source.push(curr);
      
    }

  }
  


  // mark((*this)[src], START, '$', source);
  // mark((*this)[dst], END, '#', target);
  // while(!found && !source.empty()){
  //   Voxel parent = source.front();
  //   source.pop();

  //   for(Direction d = Direction::_NORTH; d < 4; d = Direction(d+1)){
  //     if(isValid(parent.self.inc(d).z,parent.self.inc(d).y,parent.self.inc(d).x)){
  //       Voxel curr = (*this)[parent.self.inc(d)];
  //       // land in front is flat
  //       if(curr.fall == 0){
  //         if(curr.path_sign == 'x'){
  //           mark(curr,d,'$',source);
  //         }
  //         else if(curr.path_sign == '#'){
  //           found = true;
  //           source.push(curr);
  //           cout << "Found!" << endl;
  //           break;
  //         }
  //       }
        
  //     }

  //   }
  // }

 
  throw NoRoute(src, dst);
}



//  while (!found && !source.empty()) {
//     cout << source.front().self.x << endl;
//     Voxel parent = source.front();
//     source.pop();

//     for (Direction d = Direction::_NORTH; d < 4; d = Direction(d + 1)) {
//       if(isValid(parent.self.inc(d))){
//       Voxel curr = (*this)[parent.self.inc(d)];
//           try {
//             if (curr.fall == 0) {
//               if (curr.dir == DEFAULT) {
//                 mark(curr, d, '$', source);
//               }
//               else if (curr.path_sign == '#') {
//                 found = true;
//                 source.push(curr);
//                 break;
//               }
//             }
//             else if (curr.fall > 0 && curr.fall < 50000) {
//               if(curr.dir == DEFAULT){
//                 mark(curr, d, '$', source);
//                 if(isValid((curr.self.z - curr.fall), curr.self.y, curr.self.x)){
//                   curr = (*this)[{curr.self.z - curr.fall, curr.self.y, curr.self.x}];
//                   mark(curr, Direction::DOWN, '$', source);
//                 }
//               }
//               else if (curr.path_sign == '#' && isValid((curr.self.z - curr.fall), curr.self.y, curr.self.x) && map[(curr.self.z - curr.fall)][curr.self.y][curr.self.x].path_sign == '#') {
//                 found = true;
//                 source.push(curr);
//                 break;
//               }
//             }
//             else if(isValid(curr.self.inc(UP)) && isValid(parent.self.inc(UP))){
//               if (curr.fall == -1 && (*this)[curr.self.inc(UP)].fall == 0 && (*this)[parent.self.inc(UP)].fall >= 0){

//                 curr = (*this)[curr.self.inc(UP)];
//                 if(curr.dir == DEFAULT){
//                   mark(curr, d, '$', source);
//                 }
//                 else if (curr.path_sign == '#') {
//                   found = true;
//                   source.push(curr);
//                   break;
//                 }
//               }
//             }
//           } catch (const InvalidPoint &e) {
//             continue;
//           }
//       }
//   }

//   }
//   if (source.empty()) throw NoRoute(src, dst);
