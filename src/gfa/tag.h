#pragma once
#include "gfa/defines.h"
#include <cstdint>

enum GfaTagType {
  GFA_TAG_CHAR = 'A',
  GFA_TAG_INT = 'i',
  GFA_TAG_FLOAT = 'f',
  GFA_TAG_STRING = 'Z',
  GFA_TAG_JSON = 'J',
  GFA_TAG_BYTEARRAY = 'H',
  GFA_TAG_INTARRAY = 'B',
  GFA_TAG_WILDCARD = '*'
};

class GfaTag {
  public:
    GfaTag();
    GfaTag(const char _key[2], GfaTagType tagtype, const char* str);
    GfaTag(char* str, unsigned long len);
    ~GfaTag();
    
    friend ostream& operator<< (ostream &out, const GfaTag &t);
    void print(ostream &out) const;
    int16_t getID() const;
    GfaTagType getType() const;
    string asString() const;
    const char* getKey() const; //Not Null terminated!
    string getKeyAsString() const;
    
    long getIntValue();
    char* getStringValue();
    
    
  private:
    union {
      char key[2];
      int16_t id;
    };
    GfaTagType type;
    union {
      char val_char;
      long val_long;
      float val_float;
      char* val_string;
    };
};

