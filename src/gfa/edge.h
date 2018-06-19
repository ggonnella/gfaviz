#pragma once
#include "gfa/defines.h"
#include "gfa/line.h"
#include "gfa/error.h"
#include "gfa/filereader.h"
#include "gfa/types.h"
#include "gfa/alignment.h"

class GfaGraph;

class GfaEdge: public GfaReferenceableLine {
  public:
    GfaEdge();
    void fromLine(GfaFileReader* fr);
    void print(GfaVersion _version) const;
    void resolve(GfaGraph* _g);
    bool equals_dovetail(GfaEdge *e2) const;
    
    const GfaAlignment& getAlignment() const;
    unsigned long getAlignmentLength(GfaSegment* seg) const;
    
    GfaSegment* getSegment(int segnum) const;
    GfaSegment* getOppositeSegment(GfaSegment* seg) const;
    bool isInedge(int segnum) const;
    bool isInedge(const GfaSegment *seg) const;
    bool isOutedge(int segnum) const;
    bool isOutedge(const GfaSegment *seg) const;
    bool isDovetail() const;
    
    void deleteme();
    
    bool positionsSet() const;
    unsigned long getBegin(int segnum) const;
    unsigned long getEnd(int segnum) const;
    
  private:
    char type_original; //E or L or C
    GfaRef segments[2];
    GfaPos begin[2];
    GfaPos end[2];
    GfaAlignment alignment;
    
    bool positions_set;
    
  
};
