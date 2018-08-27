#include "gfa/segment.h"
#include "gfa/graph.h"

GfaEdge::GfaEdge() : GfaReferenceableLine(GFA_EDGE) {
  positions_set = false;
}
GfaEdge::~GfaEdge() {
  
}
void GfaEdge::deleteme() {
  if (isAdded()) {
    getSegment(0)->removeEdge(this);
    getSegment(1)->removeEdge(this);
  }
  delete this;
}

void GfaEdge::fromLine(GfaFileReader* fr) {
  type_original = fr->getLineType();
  if (fr->getLineType() == 'L') {
    setVersion(GFA_V1);
    fr->parseGeneric(this, "loloa", &segments[0].name, &segments[0].is_reverse, &segments[1].name, &segments[1].is_reverse, &alignment, NULL);
  } else if (fr->getLineType() == 'C') {
    setVersion(GFA_V1);
    unsigned long pos;
    fr->parseGeneric(this, "loloua", &segments[0].name, &segments[0].is_reverse, &segments[1].name, &segments[1].is_reverse, &pos, &alignment, NULL);
    //TODO
    if (alignment.isSet()) {
      positions_set = true;
      begin[0].val = pos;
      end[0].val = pos + alignment.getLength(0);
      begin[1].val = 0;
      end[1].val = alignment.getLength(1);
    }
  } else if (fr->getLineType() == 'E') {
    setVersion(GFA_V2);
    //unsigned long pos;
    fr->parseGeneric(this, "lrrppppa", &name, &segments[0], &segments[1], &begin[0], &end[0], &begin[1], &end[1], &alignment, NULL);
    positions_set = true;
    id_set = true;
    if (name[0] != '*')
      is_named = true;
  }

  setFilled();
}
ostream& operator<< (ostream &out, const GfaEdge &e) {
  e.print(out, GFA_VUNKNOWN);
  return out;
}
void GfaEdge::print(ostream &out, GfaVersion _version) const {
  if (_version == GFA_VUNKNOWN)
    _version = getVersion();

  if (_version == GFA_V1) {
    //TODO
    if (type_original == 'L') {
      out << "L\t";
      segments[0].print(out, true, '\t');
      out << '\t';
      segments[1].print(out, true, '\t');
      out << '\t';
      out << alignment;
    } else if (type_original == 'C') {
      out << "C\t";
      segments[0].print(out, true, '\t');
      out << '\t';
      segments[1].print(out, true, '\t');
      out << '\t';
      out << "0\t";
      out << alignment;
    }
  } else {
    out << "E\t" << getName() << "\t";
    segments[0].print(out,true);
    out << '\t';
    segments[1].print(out,true);
    out << '\t';
    begin[0].print(out);
    out << '\t';
    end[0].print(out,true);
    out << '\t';
    begin[1].print(out);
    out << '\t';
    end[1].print(out,true);
    out << '\t';
    out << alignment;
  }
  printTags(out);
  out << endl;
}

void GfaEdge::resolve(GfaGraph* _g) {
  GfaSegment *seg1 = _g->getSegment(segments[0].name);
  GfaSegment *seg2 = _g->getSegment(segments[1].name);
  if (seg1 == NULL) {
    throw fatal_error() << "Cannot find segment '" << segments[0].name << "'.";
  } else if (seg2 == NULL) {
    throw fatal_error() << "Cannot find segment '" << segments[1].name << "'.";
  } else {
    segments[0].resolve(seg1);
    seg1->addEdge(this);
    segments[1].resolve(seg2);
    seg2->addEdge(this);
    if (type_original == 'L') {
      //resolve positions
      if (alignment.isSet() && getSegment(0)->isLengthSet() && getSegment(1)->isLengthSet()) {
        for (int idx = 0; idx < 2; idx++) {
          if(isInedge(idx)) {
            begin[idx].val = 0;
            end[idx].val = alignment.getLength(idx);
          } else {
            begin[idx].val = getSegment(idx)->getLength() - alignment.getLength(idx);
            end[idx].val = getSegment(idx)->getLength();
          }
        }
        positions_set = true;
      }
    }
    if (positions_set) {
      for (int idx=0; idx<2; idx++) {
        if (getSegment(idx)->isLengthSet() && end[idx].val == getSegment(idx)->getLength()) {
          end[idx].is_end = true;
        }
      }
    }
  }
}

GfaSegment* GfaEdge::getSegment(int segnum) const {
  return (GfaSegment*)segments[segnum].ptr;
}

GfaSegment* GfaEdge::getOppositeSegment(GfaSegment* seg) const {
  if (seg == segments[0].ptr)
    return getSegment(1);
  else
    return getSegment(0);
}

const GfaAlignment& GfaEdge::getAlignment() const {
  return alignment;
}
unsigned long GfaEdge::getAlignmentLength(GfaSegment* seg) const {
  if (seg == getSegment(0))
    return alignment.getLength(0);
  else
    return alignment.getLength(1);
}


bool GfaEdge::equals_dovetail(GfaEdge *e2) const {
  //TODO: check if edge is actually dovetail
  const GfaEdge* e[2];
  int i,j;
  const GfaSegment* segs[2][2];
  bool in[2][2];
  bool out[2][2];

  e[0] = this;
  e[1] = e2;
  for (i=0;i<2;i++)
  {
    for (j=0;j<2;j++)
    {
      segs[i][j] = e[i]->getSegment(j);
      in[i][j] = e[i]->isInedge(j);
      out[i][j] = e[i]->isOutedge(j);
    }
  }
  if (segs[0][0] == segs[1][0] && segs[0][1] == segs[1][1])
    return (in[0][0] == in[1][0]) && (in[0][1] == in[1][1]) &&
           (out[0][0] == out[1][0]) && (out[0][1] == out[1][1]);
  else if (segs[0][0] == segs[1][1] && segs[0][1] == segs[1][0])
    return (in[0][0] == in[1][1]) && (in[0][1] == in[1][0]) &&
           (out[0][0] == out[1][1]) && (out[0][1] == out[1][0]);
  else
    return false;
}

bool GfaEdge::isInedge(int segnum) const {
  if (type_original == 'L')
    return segments[segnum].is_reverse == (segnum == 0);

  if (type_original == 'C')
    return (segnum == 1);

  return (begin[segnum].val == 0);
}
bool GfaEdge::isInedge(const GfaSegment* seg) const {
  if (seg == segments[0].ptr)
    return isInedge(0);
  else if (seg == segments[1].ptr)
    return isInedge(1);
  else
    return false;
}

bool GfaEdge::isOutedge(int segnum) const {
  if (type_original == 'L')
    return segments[segnum].is_reverse == (segnum == 1);

  if (type_original == 'C')
    return (segnum == 1);

  return end[segnum].is_end;
}
bool GfaEdge::isOutedge(const GfaSegment* seg) const {
  if (seg == segments[0].ptr)
    return isOutedge(0);
  else if (seg == segments[1].ptr)
    return isOutedge(1);
  else
    return false;
}
bool GfaEdge::isDovetail() const {
  if (type_original == 'L')
    return true;
  if (type_original == 'C')
    return false;
    
  return (isInedge(0) || isOutedge(0)) && (isInedge(1) || isOutedge(1));
}

bool GfaEdge::positionsSet() const {
  return positions_set;
}
unsigned long GfaEdge::getBegin(int segnum) const {
  if (!positions_set)
    return 0;
  return begin[segnum].val;
}
unsigned long GfaEdge::getEnd(int segnum) const {
  if (!positions_set)
    return 0;
  return end[segnum].val;
}
