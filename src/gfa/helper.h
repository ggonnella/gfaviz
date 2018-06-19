#pragma once
#include <cstring>

using namespace std;
struct cmp_str {
  bool operator()(char const *a, char const *b) {
    return strcmp(a, b) < 0;
  }
};
