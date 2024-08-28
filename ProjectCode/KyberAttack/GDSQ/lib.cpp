

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>

#include "lib.h"
#define scoreToBeat 1

using namespace std;


typedef std::map<std::string, size_t> Map;
static Map *map0;
static Map *map1;
static Map *map2;
static Map *map3;

void map_create()
{
  map0 = new Map();
  map1 = new Map();
  map2 = new Map();
  map3 = new Map();
}

void map_increment(int instance, char *k)
{
  Map *m = map0;
  switch (instance)
  {
  case 0:
    m = reinterpret_cast<Map *>(map0);
    (*m)[std::string(k)]++;
    break;
  case 1:
    m = reinterpret_cast<Map *>(map1);
    (*m)[std::string(k)]++;
    break;
  case 2:
    m = reinterpret_cast<Map *>(map2);
    (*m)[std::string(k)]++;
    break;
  case 3:
    m = reinterpret_cast<Map *>(map3);
    (*m)[std::string(k)]++;
    break;
  }
}

size_t map_get(const char *k)
{
  Map *m = reinterpret_cast<Map *>(map0);
  return (*m)[std::string(k)];
}

void map_clear() { map0->clear(); }

double bit_test(std::string s)
{
  double c = 0;
  for (int i = 0; i < s.size(); i++)
  {
    c += __builtin_popcount(s[i] & 0xff);
  }
  return c / (s.size() * 8);
}

void map_dump_single(Map *m, ofstream &mystream)
{
  int cc = 0;
  for (std::map<std::string, size_t>::iterator i = m->begin(); i != m->end();
       i++)
  {
    size_t score = i->second;
    double bitscore = bit_test(i->first);

    if (score > scoreToBeat)
    {
      mystream << std::setw(2) << std::setfill('0') << cc << ' ';
      mystream << i->first;
      mystream << ' ' << std::dec << score;
      mystream << std::endl;
      cc++;
    }
  }
}

void map_dump()
{
  ofstream mystream;
  mystream.open("mapQ.txt");
  mystream << "Map0" << std::endl;
  map_dump_single(map0, mystream);
  mystream << "Map1" << std::endl;
  map_dump_single(map1, mystream);
  mystream << "Map2" << std::endl;
  map_dump_single(map2, mystream);
  mystream << "Map3" << std::endl;
  map_dump_single(map3, mystream);
  mystream.close();
}