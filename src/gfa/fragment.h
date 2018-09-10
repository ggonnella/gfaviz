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
    virtual ~GfaFragment();
    void fromLine(GfaFileReader* fr);
    void print(ostream &out, GfaVersion _version = GFA_V2) const;
    friend ostream& operator<< (ostream &out, const GfaFragment &f);
    virtual void resolve(GfaGraph* _g);
    
    GfaSegment* getSegment() const;
    unsigned long getSegmentBegin() const;
    unsigned long getSegmentEnd() const;
    
    const string& getFragment() const;
    unsigned long getFragmentBegin() const;
    unsigned long getFragmentEnd() const;
    unsigned long getFragmentLength() const;
    bool isReverse();
    
  private:
    GfaRef segment;
    GfaRef external;
    GfaPos s_begin,
           s_end,
           f_begin,
           f_end;
    
    GfaAlignment alignment;
};
