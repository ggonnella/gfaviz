#pragma once
#include <iostream>
#include <assert.h>
#include <string>

#define GFA_FORWARD '+'
#define GFA_REVERSE '-'
#define GFA_SENTINEL '$'

#define USE_UNORDERED_MAP 1

#define GFA_SEGMENTDEFAULTLENGTH 1

using namespace std;
const string GFA_UNNAMED = "*";



enum GfaLinetype {
  GFA_HEADER,
  GFA_SEGMENT,
  GFA_EDGE,
  GFA_GROUP,
  GFA_FRAGMENT,
  GFA_GAP
};
enum GfaVersion {
  GFA_VUNKNOWN,
  GFA_V1,
  GFA_V2 
};

