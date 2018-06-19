#pragma once
#include <string>
#include "gfa/defines.h"

class GfaLine;

class GfaVariance {
  public:
    GfaVariance();
    void set(unsigned long _val);
    void print() const;
    unsigned long val;
    bool is_set;
};

class GfaRef {
  public:
    GfaRef();
    void resolve(GfaLine *_r);
    const string& getName() const;
    void print(bool print_orientation = true, char delimiter = 0) const;
    
    string name;
    GfaLine *ptr;
    GfaLinetype type;
    bool is_resolved;
    bool is_reverse;
};

class GfaPos {
  public:
    GfaPos();
    void print(bool include_sentinel = true) const;
    unsigned long val;
    bool is_end;
};
