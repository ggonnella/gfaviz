#pragma once
#include "gfa/defines.h"
//#include "gfa/line.h"
#include "gfa/segment.h"
#include "gfa/edge.h"
#include "gfa/fragment.h"
#include "gfa/gap.h"
#include "gfa/group.h"
#include "gfa/helper.h"
#include <string>
#include <map>
#include <unordered_map>
#include <unordered_set>

//try: https://github.com/Cyan4973/xxHash/blob/dev/xxhash.h
#if USE_UNORDERED_MAP
  typedef unordered_map<string, GfaSegment*> GfaSegmentMap;
  typedef unordered_map<string, GfaEdge*> GfaEdgeMap;
  typedef unordered_map<string, GfaGroup*> GfaGroupMap;
  typedef unordered_map<string, GfaGap*> GfaGapMap;
#else
  typedef map<string, GfaSegment*> GfaSegmentMap;
  typedef map<string, GfaEdge*> GfaEdgeMap;
  typedef map<string, GfaGroup*> GfaGroupMap;
  typedef map<string, GfaGap*> GfaGapMap;
#endif



typedef vector<GfaFragment*> GfaFragmentVector;
typedef vector<string> GfaCommentVector;

class GfaGraph
{
  public:
    GfaGraph();
    void open(const char* filename);
    ~GfaGraph();
    
    //int fromFile(char *filename, GfaError *err);
    
    GfaVersion getVersion() const;
    void setVersion(GfaVersion _v);
    
    void print(GfaVersion _version = GFA_VUNKNOWN) const;
    
    void addComment(const string& _comment);
    void addLine(GfaLine* line);
    GfaLine* getLine(const string& name) const;
    
    void remove(GfaLine *line);
    //void erase(GfaEdge* edge);
    //void erase(GfaSegment* seg);
    //void erase(GfaGroup* group);
    
    const GfaSegmentMap& getSegments() const;
    unsigned long getSegmentCount() const;
    GfaSegment* getSegment(const string& name) const;
    
    const GfaEdgeMap& getEdges() const;
    unsigned long getEdgeCount() const;
    GfaEdge* getEdge(const string& name) const;
    
    const GfaGroupMap& getGroups() const;
    unsigned long getGroupCount() const;
    GfaGroup* getGroup(const string& name) const;
    
    const GfaGapMap& getGaps() const;
    unsigned long getGapCount() const;
    GfaGap* getGap(const string& name) const;
    
    const GfaFragmentVector& getFragments() const;
    unsigned long getFragmentCount() const;
    
    void removeDuplicateDovetails();
    void mergeLinearPaths(bool appendNames = false);
    
    void addTag(GfaTag* tag);
    bool hasTag(const char key[2], GfaTagType tagtype = GFA_TAG_WILDCARD) const;
    GfaTag* getTag(const char key[2], GfaTagType tagtype = GFA_TAG_WILDCARD) const;
    
    /* hidden public functions */
    void rehash(unsigned long n_segs, unsigned long n_edges, unsigned long n_groups, unsigned long n_gaps);
    void addSegment(GfaSegment *seg);
    void addEdge(GfaEdge *edge);
    void addFragment(GfaFragment *frag);
    void addGap(GfaGap *gap);
    void addGroup(GfaGroup *group, bool resolve=true);
    
  private:
    GfaLine header = GfaLine(GFA_HEADER);
    GfaVersion version;
    GfaCommentVector comments;
    GfaSegmentMap segments;
    GfaEdgeMap edges;
    GfaGroupMap groups;
    GfaGapMap gaps;
    GfaFragmentVector fragments;
};
