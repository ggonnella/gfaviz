#pragma once
#include "gfa/defines.h"
#include "gfa/line.h"
#include "gfa/error.h"
#include "gfa/filereader.h"
#include "gfa/types.h"
#include "gfa/alignment.h"

class GfaGraph;
class GfaSegment;

class GfaFragment: public GfaLine {
  public:
    GfaFragment();
    void fromLine(GfaFileReader* fr);
    void print() const;
    void resolve(GfaGraph* _g);
    
    GfaSegment* getSegment() const;
    unsigned long getSegmentBegin() const;
    unsigned long getSegmentEnd() const;
    
    const string& getFragment() const;
    unsigned long getFragmentBegin() const;
    unsigned long getFragmentEnd() const;
    
  private:
    GfaRef segment;
    GfaRef external;
    GfaPos s_begin,
           s_end,
           f_begin,
           f_end;
    
    GfaAlignment alignment;
};
