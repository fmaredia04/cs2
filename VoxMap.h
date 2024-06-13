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
    Point parent;
    Point self;
    int fall;

    public:
      Voxel(int f){
        fall = f;
        state = UNSEEN;
        parent.x = -1;
        parent.y = -1;
        parent.z = -1;
      }
      Voxel(int f, int z, int y, int x){
        fall = f;
        state = UNSEEN;
        self.x = x;
        self.y = y;
        self.z = z;
        parent.x = -1;
        parent.y = -1;
        parent.z = -1;
      }
      Voxel(){
        fall = -1;
        state = UNSEEN;
        parent.x = -1;
        parent.y = -1;
        parent.z = -1;
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


public:
  VoxMap(std::istream& stream)
  {
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
  ~VoxMap();

  Route route(Point src, Point dst);
};

#endif