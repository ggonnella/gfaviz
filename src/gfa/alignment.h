#pragma once
#include "gfa/defines.h"
#include <vector>
#include "gfa/error.h"

enum GfaCigarOpType {
  GFA_CIGAR_SUBSTITUTION = 'M',
  GFA_CIGAR_INSERTION = 'I',
  GFA_CIGAR_DELETION = 'D',
  GFA_CIGAR_PADDING = 'P',
  GFA_CIGAR_SKIPPED = 'N',
  GFA_CIGAR_SOFTCLIPPING = 'S',
  GFA_CIGAR_HARDCLIPPING = 'H',
  GFA_CIGAR_MATCH = '=',
  GFA_CIGAR_MISMATCH = 'X'
};

struct GfaCigarOp {
  GfaCigarOpType type;
  unsigned long count;
};

class GfaAlignment {
  public:
    GfaAlignment();
    ~GfaAlignment();
    void createFromCigarString(char *str, unsigned long len);
    friend ostream& operator<< (ostream &out, const GfaAlignment &a);
    void print(ostream &out) const;
    unsigned long getLength(int idx=0) const;
    bool isSet() const;
  private:
    bool is_set;
    unsigned long lengths[2];
    vector<GfaCigarOp> ops;
    
};
