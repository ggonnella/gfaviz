#include "gfa/sequence.h"

GfaSequence::GfaSequence() {
  is_set = false;
  seq = "*";
}
GfaSequence::GfaSequence(const string& _seq) {
  is_set = true;
  seq = _seq;
}
GfaSequence::~GfaSequence() {
  
}

void GfaSequence::createFromString(char *str, unsigned long len) {
  if (len == 1 && str[0] == '*') {
    is_set=false;
    return;
  }
  is_set = true;
  seq.assign(str, len);
}

void GfaSequence::print() const {
  if (is_set)
    cout << seq;
  else
    cout << '*';
}

unsigned long GfaSequence::getLength() const {
  if (is_set)
    return seq.length();
  else
    return GFA_SEGMENTDEFAULTLENGTH;
}

bool GfaSequence::isSet() const {
  return is_set;
}

const string& GfaSequence::getString() const {
  return seq;
}
static char comprev(char n) {
  switch(n) {
    case 'A': return 'T';
    case 'a': return 't';
    case 'C': return 'G';
    case 'c': return 'g';
    case 'G': return 'C';
    case 'g': return 'c';
    case 'T': return 'A';
    case 't': return 'a';
  }
  throw fatal_error() << n << " is not a valid base.";
}
const string GfaSequence::getStringCompRev() const {
  size_t len = seq.size();
  string seqrev = seq;
  
  for (size_t idx=0; idx < len; idx++) {
    seqrev[idx] = comprev(seq[len-idx-1]);
  }
  return seqrev;
}
