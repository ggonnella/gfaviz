#include "gfa/graph.h"

struct GfaLinearPath {
  GfaSegment *start_seg, *end_seg;
  vector<GfaEdge*> edges;
};

static bool canBeMerged(GfaEdge* e) {
  return (e->getSegment(0)->getParallelEdges(e).size() == 1 &&
          e->getSegment(1)->getParallelEdges(e).size() == 1);
}

static bool isStartSegment(GfaSegment *s) {
  bool startin = (s->getInedges().size() == 1 && canBeMerged(s->getInedges()[0]));
  bool startout = (s->getOutedges().size() == 1 && canBeMerged(s->getOutedges()[0]));
  return (startin ^ startout);  
}

static const string getMergedName(const GfaLinearPath& path) {
  GfaSegment *next_seg = path.start_seg;
  string mergedname = next_seg->getName();
  if (path.edges[0]->isInedge(next_seg)) {
    mergedname.push_back('^');
  }
  for (size_t idx=0; idx < path.edges.size(); idx++) {
    next_seg = path.edges[idx]->getOppositeSegment(next_seg);
    mergedname.push_back('_');
    mergedname.append(next_seg->getName());
    if (path.edges[idx]->isOutedge(next_seg)) {
      mergedname.push_back('^');
    }
  }
  return mergedname;
}

static const GfaSequence getMergedSequence(const GfaLinearPath& path) {
  string seq = "";
  GfaSegment *next_seg = path.start_seg;
  
  if (!next_seg->getSequence().isSet() || !path.edges[0]->getAlignment().isSet())
    return GfaSequence();
    
  if (path.edges[0]->isInedge(next_seg)) {
    seq = next_seg->getSequence().getStringCompRev();
  } else {
    seq = next_seg->getSequence().getString();
  }
  
  for (size_t idx=0; idx < path.edges.size(); idx++) {
    next_seg = path.edges[idx]->getOppositeSegment(next_seg);
    if (!next_seg->getSequence().isSet() || !path.edges[idx]->getAlignment().isSet())
      return GfaSequence();
    string t_seq;
    if (path.edges[idx]->isOutedge(next_seg)) {
      t_seq = next_seg->getSequence().getStringCompRev();
    } else {
      t_seq = next_seg->getSequence().getString();
    }
    unsigned long alignlen = path.edges[idx]->getAlignmentLength(next_seg);
    //cerr << alignlen << endl << endl;
    seq.append(t_seq.substr(alignlen));
  }
  return GfaSequence(seq);
}

static void deletePathFromGraph(const GfaLinearPath& path) {
  //all but the first one
  GfaSegment *next_seg = path.start_seg;
  next_seg = path.edges[0]->getOppositeSegment(next_seg);
  for (size_t idx=1; idx < path.edges.size(); idx++) {
    GfaSegment *t_seg = next_seg;
    next_seg = path.edges[idx]->getOppositeSegment(next_seg);
    t_seg->remove();
  }
  next_seg->remove();
}

static void mergePath(const GfaLinearPath& path, const string& name) {
  //cout << getMergedName(path) << endl << endl;
  path.start_seg->setName(name);
  path.start_seg->setSequence(getMergedSequence(path));
  
  deletePathFromGraph(path);
}

static const GfaLinearPath traversePath(GfaSegment *seg) {
  //Only traverses in one direction -> needs a start segment
  GfaLinearPath path;
  path.start_seg = seg;
  GfaEdge *next_edge = NULL;
  GfaSegment *next_seg = seg;
  if (seg->getInedges().size() == 1 && canBeMerged(seg->getInedges()[0]))
    next_edge = seg->getInedges()[0];
  else if (seg->getOutedges().size() == 1 && canBeMerged(seg->getOutedges()[0]))
    next_edge = seg->getOutedges()[0];

  while (next_edge) {
    path.edges.push_back(next_edge);
    next_seg = next_edge->getOppositeSegment(next_seg);
    const vector<GfaEdge*>& o_edges = next_seg->getOppositeEdges(next_edge);
    if (o_edges.size() == 1 && canBeMerged(o_edges[0])) {
      next_edge = o_edges[0];
    } else {
      next_edge = NULL;
    }
  }
  path.end_seg = next_seg; 
  return path;
}

void GfaGraph::mergeLinearPaths(bool appendNames) {
  removeDuplicateDovetails();
  vector<GfaLinearPath> paths;
  unordered_set<GfaSegment*> start_segments;
  for (auto it : segments) {
    GfaSegment* s = it.second;
    if (isStartSegment(s) && start_segments.count(s) == 0) {
      const GfaLinearPath path = traversePath(s);
      paths.push_back(path);
      start_segments.emplace(path.start_seg);
      start_segments.emplace(path.end_seg);
    }
  }
  unsigned long cnt=0;
  for (auto path : paths) {
    cnt++;
    const string name = (appendNames ? getMergedName(path) : string("merged_").append(to_string(cnt)));
    mergePath(path, name);
  }
}



