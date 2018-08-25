#include <assert.h>
#include <cstring>
#include "gfa/graph.h"
#include "gfa/filereader.h"
#include "gfa/error.h"

GfaGraph::GfaGraph() {
  
}
void GfaGraph::open(const char* filename) {
  GfaFileReader fr;
  
  setVersion(GFA_VUNKNOWN);
  
  fr.open(filename);
  
  vector<GfaSegment*> t_segs;
  vector<GfaEdge*> t_edges;
  vector<GfaFragment*> t_frags;
  vector<GfaGap*> t_gaps;
  vector<GfaGroup*> t_groups;
  while (fr.readNextLine()) {
    switch(fr.getLineType()) {
      case 'H': {
        fr.parseGeneric(&header, "", NULL);
        break;
      }
      case 'S': {
        GfaSegment *seg = new GfaSegment();
        seg->fromLine(&fr);
        t_segs.push_back(seg);
        break;
      }
      case 'E':
      case 'C':
      case 'L': {
        GfaEdge *edge = new GfaEdge();
        edge->fromLine(&fr);
        t_edges.push_back(edge);
        break;
      }
      case 'F': {
        GfaFragment *frag = new GfaFragment();
        frag->fromLine(&fr);
        t_frags.push_back(frag);
        break;
      }
      case 'G': {
        GfaGap *gap = new GfaGap();
        gap->fromLine(&fr);
        t_gaps.push_back(gap);
        break;
      }
      case 'O':
      case 'U':
      case 'P': {
        GfaGroup *group = new GfaGroup();
        group->fromLine(&fr);
        t_groups.push_back(group);
        break;
      }
      default: {
        addComment(string(fr.getLine()));
      }
    }
  }
  rehash(t_segs.size(),t_edges.size(),t_groups.size(),t_gaps.size());
  for (size_t idx=0; idx < t_segs.size(); idx++)
    addLine(t_segs[idx]);
  for (size_t idx=0; idx < t_edges.size(); idx++)
    addLine(t_edges[idx]);
  for (size_t idx=0; idx < t_groups.size(); idx++)
    addGroup(t_groups[idx], false); // Noch nicht resolven, weil Groups andere Groups referenzieren können.
  for (size_t idx=0; idx < t_frags.size(); idx++)
    addLine(t_frags[idx]);
  for (size_t idx=0; idx < t_gaps.size(); idx++)
    addLine(t_gaps[idx]);  
    
  for (size_t idx=0; idx < t_groups.size(); idx++)
    t_groups[idx]->resolve(this);
}

GfaGraph::~GfaGraph() {
  for (auto s : segments)
    delete s.second;
  for (auto e : edges)
    delete e.second;
  for (auto g : groups)
    delete g.second;
  for (auto f : fragments)
    delete f;
  for (auto g : gaps)
    delete g.second;
}

void GfaGraph::addLine(GfaLine* line) {
  if (!line->isFilled()) {
    throw fatal_error() << "Cannot add empty line to graph.";
  }
  switch (line->getType()) {
    case GFA_SEGMENT:
      addSegment((GfaSegment*)line);
      break;
    case GFA_EDGE:
      addEdge((GfaEdge*)line);
      break;
    case GFA_GROUP:
      addGroup((GfaGroup*)line);
      break;
    case GFA_FRAGMENT:
      addFragment((GfaFragment*)line);
      break;
    case GFA_GAP:
      addGap((GfaGap*)line);
      break;
  }
  line->setGraph(this);
}
void GfaGraph::addComment(const string& _comment) {
  comments.push_back(_comment);
}

GfaVersion GfaGraph::getVersion() const {
  return version;
}
void GfaGraph::setVersion(GfaVersion _v) {
  version = _v;
}

void GfaGraph::addSegment(GfaSegment *seg) {
  const string& id = seg->getID();
  if (getLine(id) != NULL) {
    throw fatal_error() << "Found duplicate ID '" << id << "'.";
  } else {
    segments.emplace(id, seg);
    seg->setGraph(this);
  }
}

const GfaSegmentMap& GfaGraph::getSegments() const {
  return segments;
}

unsigned long GfaGraph::getSegmentCount() const {
  return segments.size();
}

void GfaGraph::addFragment(GfaFragment *frag) {
  frag->resolve(this);
  frag->setGraph(this);
  fragments.push_back(frag);
}

const GfaFragmentVector& GfaGraph::getFragments() const {
  return fragments;
}

unsigned long GfaGraph::getFragmentCount() const {
  return fragments.size();
}

void GfaGraph::addGap(GfaGap *gap) {
  const string& id = gap->getID();
  if (getLine(id) != NULL) {
    throw fatal_error() << "Found duplicate ID '" << id << "'.";
  } else {
    gap->resolve(this);
    gap->setGraph(this);
    gaps.emplace(id, gap);
  }
}

const GfaGapMap& GfaGraph::getGaps() const {
  return gaps;
}
unsigned long GfaGraph::getGapCount() const {
  return gaps.size();
}

void GfaGraph::addEdge(GfaEdge *edge) {
  const string& id = edge->getID();
  if (getLine(id) != NULL) {
    throw fatal_error() << "Found duplicate ID '" << id << "'.";
  } else {
    edge->resolve(this);
    edge->setGraph(this);
    edges.emplace(id, edge);
  }
}

const GfaEdgeMap& GfaGraph::getEdges() const {
  return edges;
}

unsigned long GfaGraph::getEdgeCount() const {
  return edges.size();
}



void GfaGraph::addGroup(GfaGroup *group, bool resolve) {
  const string& id = group->getID();
  if (getLine(id) != NULL) {
    throw fatal_error() << "Found duplicate ID '" << id << "'.";
  } else {
    if (resolve) {
      group->resolve(this);
    } 
    group->setGraph(this);
    groups.emplace(id, group);
  }
}

const GfaGroupMap& GfaGraph::getGroups() const {
  return groups;
}

unsigned long GfaGraph::getGroupCount() const {
  return groups.size();
}

void GfaGraph::print(GfaVersion _version) const {
  header.print(_version);
  for (auto c : comments)
    cout << c << endl;
  for (auto s : segments)
    s.second->print(_version);
  for (auto e : edges)
    e.second->print(_version);
  for (auto g : groups)
    g.second->print(_version);
  if (_version != GFA_V1) {
    for (auto f : fragments)
      f->print();
    for (auto g : gaps)
      g.second->print();
  }
}

/*int GfaGraph::resolveAll(GfaError *err) {
  int had_err;
  had_err = resolveEdges(err);
  
  return had_err;
}*/

/*int GfaGraph::resolveEdges(GfaError *err) {
  int had_err = 0;
  for (auto e : edges) {
    had_err = e.second->resolve(err);
    if (had_err)
      break;
  }
  return had_err;
}*/

GfaLine* GfaGraph::getLine(const string& name) const {
  GfaLine *line;
  line = getSegment(name);
  if (line)
    return line;
  line = getEdge(name);
  if (line)
    return line;
  line = getGroup(name);
  if (line)
    return line;
  line = getGap(name);
  if (line)
    return line;
    
  return NULL;  
}

GfaSegment* GfaGraph::getSegment(const string& name) const {
  GfaSegmentMap::const_iterator it = segments.find(name);
  if(it != segments.end())
    return it->second;
  else
    return NULL;
}
GfaEdge* GfaGraph::getEdge(const string& name) const {
  GfaEdgeMap::const_iterator it = edges.find(name);
  if(it != edges.end())
    return it->second;
  else
    return NULL;
}
GfaGroup* GfaGraph::getGroup(const string& name) const {
  GfaGroupMap::const_iterator it = groups.find(name);
  if(it != groups.end())
    return it->second;
  else
    return NULL;
}
GfaGap* GfaGraph::getGap(const string& name) const {
  GfaGapMap::const_iterator it = gaps.find(name);
  if(it != gaps.end())
    return it->second;
  else
    return NULL;
}

void GfaGraph::addTag(GfaTag* tag) {
  header.addTag(tag);
}
bool GfaGraph::hasTag(const char key[2], GfaTagType tagtype) const {
  return header.hasTag(key,tagtype);
}
GfaTag* GfaGraph::getTag(const char key[2], GfaTagType tagtype) const {
  return header.getTag(key,tagtype);
}
const vector<GfaTag*>& GfaGraph::getTags() const {
  return header.getTags();
}

void GfaGraph::remove(GfaLine *line) {
  switch (line->getType()) {
    case GFA_SEGMENT: {
      segments.erase(line->getID());
      break;
    }
    case GFA_EDGE: {
      edges.erase(line->getID());
      break;
    }
    case GFA_GROUP: {
      groups.erase(line->getID());
      break;
    }
    case GFA_GAP: {
      gaps.erase(line->getID());
      break;
    }
  }
  line->deleteme();  
}

void GfaGraph::rehash(unsigned long n_segs, unsigned long n_edges, unsigned long n_groups, unsigned long n_gaps) {
  #if USE_UNORDERED_MAP
    segments.rehash(n_segs*1.3);
    edges.rehash(n_edges*1.3);
    groups.rehash(n_groups*1.3);
    gaps.rehash(n_gaps*1.3);
  #endif
}

static inline const vector<GfaEdge*>& getEdgeVector(GfaSegment* s, int idx) {
  if (idx==0)
    return s->getInedges();
  else
    return s->getOutedges();
}
void GfaGraph::removeDuplicateDovetails() {
  for (auto s : segments) {
    for (int v_idx = 0; v_idx < 2; v_idx++) {
      const vector<GfaEdge*>& s_edges = getEdgeVector(s.second, v_idx);
      for (size_t i=0; i < s_edges.size(); i++) {
        for (size_t j=i+1; j < s_edges.size(); j++) {
          if (s_edges[i]->equals_dovetail(s_edges[j])) {
            s_edges[i]->remove();
            break;
          }
        }
      }
    }
  }
}
