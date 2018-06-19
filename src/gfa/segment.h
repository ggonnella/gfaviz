#pragma once
#include "gfa/defines.h"
#include "gfa/line.h"
#include "gfa/error.h"
#include "gfa/filereader.h"
#include "gfa/types.h"
#include "gfa/sequence.h"

class GfaGraph;
class GfaEdge;
class GfaFragment;
class GfaGap;

class GfaSegment: public GfaReferenceableLine {
  public:
    GfaSegment();
    //GfaSegment()
    void fromLine(GfaFileReader* fr);
    void print(GfaVersion _version) const;
    
    void addFragment(GfaFragment* frag);
    const vector<GfaFragment*>& getFragments() const;

    void addGap(GfaGap* gap);
    const vector<GfaGap*>& getGaps() const;

    void addEdge(GfaEdge *edge);
    void removeEdge(GfaEdge *edge);
    const vector<GfaEdge*>& getEdges() const;
    const vector<GfaEdge*>& getInedges() const;
    const vector<GfaEdge*>& getOutedges() const;
    const vector<GfaEdge*>& getParallelEdges(GfaEdge* edge) const;
    const vector<GfaEdge*>& getOppositeEdges(GfaEdge* edge) const;
    
    
    
    unsigned long getLength() const;
    bool isLengthSet() const;
    
    bool isInedge(GfaEdge* edge) const;
    bool isOutedge(GfaEdge* edge) const;
    
    void deleteme();
    
    const GfaSequence& getSequence() const;
    void setSequence(const GfaSequence& _seq);
  
  private:
    GfaSequence sequence;
    unsigned long length;
    bool length_set;
    vector<GfaFragment*> fragments;
    vector<GfaGap*> gaps;
    vector<GfaEdge*> edges;
    vector<GfaEdge*> inedges;
    vector<GfaEdge*> outedges;
    
  
};
