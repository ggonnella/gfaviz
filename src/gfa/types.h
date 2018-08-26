#pragma once
#include <string>
#include "gfa/defines.h"

class GfaLine;

class GfaVariance {
  public:
    GfaVariance();
    void set(unsigned long _val);
    friend ostream& operator<< (ostream &out, const GfaVariance &v);
    void print(ostream &out) const;
    unsigned long val;
    bool is_set;
};

class GfaRef {
  public:
    GfaRef();
    void resolve(GfaLine *_r);
    const string& getName() const;
    friend ostream& operator<< (ostream &out, const GfaRef &r);
    void print(ostream &out, bool print_orientation = true, char delimiter = 0) const;
    
    string name;
    GfaLine *ptr;
    GfaLinetype type;
    bool is_resolved;
    bool is_reverse;
};

class GfaPos {
  public:
    GfaPos();
    friend ostream& operator<< (ostream &out, const GfaPos &p);
    void print(ostream &out, bool include_sentinel = true) const;
    unsigned long val;
    bool is_end;
};
