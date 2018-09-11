#include "gfa/alignment.h"

GfaAlignment::GfaAlignment() {
  is_set = false;
  lengths[0] = 0;
  lengths[1] = 0;
}
GfaAlignment::~GfaAlignment() {
  //if (cigar)
    //free(cigar);
}

void GfaAlignment::createFromCigarString(char *str, unsigned long len) {
  unsigned long idx;
  bool char_found = false;
  bool digit_found = false;
  bool last_start = 0;
  if (len == 1 && str[0] == '*') { /* 1st alternative: '*' */
    is_set = false;
    return;
  }
  else {/* Just allowing MIDP as of GFA2 */
    for (idx = 0; idx < len; idx++) {
      if ((str[idx] == 'M' || str[idx] == 'I' || str[idx] == 'D' || str[idx] == 'P') && digit_found == true) { /* MIDP*/
        unsigned long numops = atol(str+last_start);
        if (str[idx] == 'M') {
          lengths[0]+=numops;
          lengths[1]+=numops;
        } else if (str[idx] == 'I') {
          lengths[1]+=numops;
        } else if (str[idx] == 'D') {
          lengths[0]+=numops;
        } else if (str[idx] == 'P') {
          //TODO: ???
        } else if (str[idx] == 'X') {
          //TODO: ???
        } else if (str[idx] == '=') {
          //TODO: ???
        } else if (str[idx] == 'S') {
          //TODO: ???
        } else if (str[idx] == 'H') {
          //TODO: ???
        }
        GfaCigarOp op;
        op.type = (GfaCigarOpType)str[idx];
        op.count = numops;
        ops.push_back(op);
        char_found = true;
        digit_found = false;
        last_start = idx+1;
      } else if (isdigit(str[idx])) {
        digit_found = true;
        char_found = false;
      } else {
        throw fatal_error() << "Invalid Cigar string '" << str << "'.";
      }
    }
  }
  if (!char_found)  {
    throw fatal_error() << "Invalid Cigar string '" << str << "'.";
  }
    
  is_set = true;
}
ostream& operator<< (ostream &out, const GfaAlignment &a) {
  a.print(out);
  return out;
}
void GfaAlignment::print(ostream &out) const {
  if (is_set) {
    for (GfaCigarOp op : ops) {
      out << op.count << (char)op.type;
    }
  } else {
    out << '*';
  }
}

unsigned long GfaAlignment::getLength(int idx) const {
  return lengths[idx];
}

bool GfaAlignment::isSet() const {
  return is_set;
}
