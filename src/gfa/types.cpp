#include "gfa/types.h"
#include "gfa/line.h"

GfaVariance::GfaVariance() {
  is_set = false;
}
void GfaVariance::set(unsigned long _val) {
  is_set = true;
  val=_val;
}
ostream& operator<< (ostream &out, const GfaVariance &v) {
  v.print(out);
  return out;
}
void GfaVariance::print(ostream &out) const {
  if (!is_set)
    out << '*';
  else
    out << val;
}

GfaPos::GfaPos() {
  val = 0;
  is_end = false;
}

GfaRef::GfaRef() {
  is_resolved = false;
}
void GfaRef::resolve(GfaLine *_r) {
  ptr = _r;
  type = ptr->getType();
  is_resolved = true;
}
const string& GfaRef::getName() const{
  if (is_resolved)
    return ptr->getName();
  else
    return name;
}
ostream& operator<< (ostream &out, const GfaRef &r) {
  r.print(out);
  return out;
}
void GfaRef::print(ostream &out,bool print_orientation, char delimiter) const {
  out << getName();
  if (print_orientation) {
    if (delimiter != 0)
      out << delimiter;
    out << (is_reverse ? '-' : '+');
  }
}

ostream& operator<< (ostream &out, const GfaPos &p) {
  p.print(out);
  return out;
}
void GfaPos::print(ostream &out, bool include_sentinel) const {
  out << val;
  if (include_sentinel && is_end)
    out << (char)GFA_SENTINEL;
}


