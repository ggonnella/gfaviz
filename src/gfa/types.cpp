#include "gfa/types.h"
#include "gfa/line.h"

GfaVariance::GfaVariance() {
  is_set = false;
}
void GfaVariance::set(unsigned long _val) {
  is_set = true;
  val=_val;
}
void GfaVariance::print() const {
  if (!is_set)
    cout << '*';
  else
    cout << val;
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
void GfaRef::print(bool print_orientation, char delimiter) const {
  cout << getName();
  if (print_orientation) {
    if (delimiter != 0)
      cout << delimiter;
    cout << (is_reverse ? '-' : '+');
  }
}

void GfaPos::print(bool include_sentinel) const {
  cout << val;
  if (include_sentinel && is_end)
    cout << (char)GFA_SENTINEL;
}


