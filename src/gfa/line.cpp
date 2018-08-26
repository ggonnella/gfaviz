#include "gfa/line.h"
#include "gfa/graph.h"

GfaLine::GfaLine(GfaLinetype _t) {
  type = _t;
  is_named = false;
  id_set = false;
  is_filled = false;
  is_added = false;
  version = GFA_VUNKNOWN;
  name = "";
}

GfaLine::~GfaLine() {
  
}
ostream& operator<< (ostream &out, const GfaLine &line) {
  line.print(out,GFA_VUNKNOWN);
  return out;
}
void GfaLine::print(ostream &out,GfaVersion version) const {
  out << 'H';
  printTags(out);
  out << endl;
}

void GfaLine::setGraph(GfaGraph* _g) {
  graph = _g;
  setAdded();
}
bool GfaLine::isFilled() {
  return is_filled;
}
void GfaLine::setFilled() {
  is_filled = true;
}
bool GfaLine::isAdded() {
  return is_added;
}
void GfaLine::setAdded() {
  is_added = true;
}
GfaVersion GfaLine::getVersion() const {
  return version;
}
void GfaLine::setVersion(GfaVersion _v) {
  version = _v;
}

GfaLinetype GfaLine::getType() const {
  return type;
}
void GfaLine::setType(GfaLinetype _t) {
  type = _t;
}

const string& GfaLine::getID() {
  if (!id_set || (name.length() == 1 && name[0] == '*')) {
    name = getIDGeneric();
    id_set = true;
    is_named = false;
  }
  return name;
}
const string& GfaLine::getName() const {
  if (is_named)
    return name;
  else
    return GFA_UNNAMED;
}

string GfaLine::getIDGeneric() const {
  const GfaLine *addr = this;
  return string((char*)&addr, sizeof(addr));
}


void GfaLine::setName(string _n) {
  name = _n;
  is_named = true;
  id_set = true;
}
bool GfaLine::hasName() const {
  return is_named;
}
void GfaLine::remove() {
  if (isAdded()) {
    graph->remove(this);
  }
}

void GfaReferenceableLine::addGroup(GfaGroup *group) {
  groups.push_back(group);
}
void GfaReferenceableLine::removeGroup(GfaGroup *group) {
  for (size_t idx=0; idx < groups.size(); idx++) {
    if (groups[idx] == group) {
      groups.erase(groups.begin()+idx);
      idx--;
    }
  }
}
const vector<GfaGroup*>& GfaReferenceableLine::getGroups() const {
  return groups;
}

void GfaLine::addTag(GfaTag* tag) {
  tags.push_back(tag);
}

void GfaLine::printTags(ostream &out) const {
  for (GfaTag* tag : tags) {
    out << '\t';
    tag->print(out);
  }
}

void GfaLine::removeTag(const char key[2], GfaTagType tagtype) {
  int16_t tagid = *((int16_t*)key);
  for (size_t idx = 0; idx < tags.size(); idx++) {
    GfaTag* tag = tags[idx];
    if (tagid == tag->getID() && (tagtype == GFA_TAG_WILDCARD || 
                                  tagtype == tag->getType())) {
      tags.erase(tags.begin()+idx);
      delete tag;
      idx--;
    }
  }
}

bool GfaLine::hasTag(const char key[2], GfaTagType tagtype) const {
  int16_t tagid = *((int16_t*)key);
  for (GfaTag* tag : tags) {
    if (tagid == tag->getID() && (tagtype == GFA_TAG_WILDCARD || 
                                  tagtype == tag->getType())) {
      return true;
    }
  }
  return false;
}
GfaTag* GfaLine::getTag(const char key[2], GfaTagType tagtype) const {
  int16_t tagid = *((int16_t*)key);
  for (GfaTag* tag : tags) {
    if (tagid == tag->getID() && (tagtype == GFA_TAG_WILDCARD || 
                                  tagtype == tag->getType())) {
      return tag;
    }
  }
  return NULL;
}
const vector<GfaTag*>& GfaLine::getTags() const {
  return tags;
}
