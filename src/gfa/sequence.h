#pragma once
#include "gfa/defines.h"
#include <string>
#include "gfa/error.h"

class GfaSequence {
  public:
    GfaSequence();
    GfaSequence(const string& _seq);
    ~GfaSequence();
    void createFromString(char *str, unsigned long len);
    void print() const;
    unsigned long getLength() const;
    bool isSet() const;
    const string& getString() const;
    const string getStringCompRev() const;
    
  private:
    bool is_set;
    string seq;
};
