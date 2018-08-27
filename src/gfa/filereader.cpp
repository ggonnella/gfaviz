#include "gfa/filereader.h"
#include <cstring>
#include <iostream>
#include <assert.h>
#include "gfa/validate.h"
#include "gfa/types.h"
#include "gfa/defines.h"
#include "gfa/graph.h"

#define GFAREADERBUFSIZE BUFSIZ

using namespace std;

GfaFileReader::GfaFileReader() {
  buf = (char*)malloc(sizeof *buf * GFAREADERBUFSIZE);
  buf_size = GFAREADERBUFSIZE;
  filebuf = (char*)malloc(sizeof *filebuf * GFAREADERBUFSIZE);
  filebuf_size = GFAREADERBUFSIZE;
}

GfaFileReader::~GfaFileReader() {
  if (buf)
    free(buf);
  if (filebuf)
    free(filebuf);
  if (fp)
    fclose(fp);
}

void GfaFileReader::open(const char *filename) {
  fp = fopen(filename, "rb");
  if (!fp) {
    throw fatal_error() << "File '" << filename << "' not found.";
  }
  line_num = 0;
  filebuf_pos = 0;
  filebuf_left = fread(filebuf, 1, filebuf_size, fp);
}
char GfaFileReader::getNext() {
  if (filebuf_pos >= filebuf_left) {
    if (filebuf_pos >= filebuf_size) {
      filebuf_left = fread(filebuf, 1, filebuf_size, fp);
      filebuf_pos = 0;
    } else {
      return EOF;
    }
  }
  //cout << filebuf[filebuf_pos] << endl;
  return filebuf[filebuf_pos++];
}

bool GfaFileReader::readNextLine() {
  columns.clear();
  char cc;
  unsigned long buf_pos = 0;
  unsigned long last_field = 0;
  do {
    //cc = fgetc(fp);
    cc = getNext();
    if (buf_pos >= buf_size) {
      buf_size *= 2;
      buf = (char*)realloc(buf, buf_size);
    }
    buf[buf_pos] = cc;
    if (cc=='\t') {
      columns.push_back(GfaColumn{last_field, buf_pos-last_field}); //make_pair(last_field, buf_pos));
      last_field = buf_pos + 1;
    }
    buf_pos++;
  } while (cc != '\r' && cc != '\n' && cc != EOF);
  buf[buf_pos-1] = '\0';
  columns.push_back(GfaColumn{last_field, buf_pos-last_field-1});
  
  field_count = columns.size();
  if (cc == '\r') 
    (void)fgetc(fp);  
  line_num++;
  return (buf_pos > 1);
}
unsigned long GfaFileReader::fieldCount() {
  return field_count;
}
char GfaFileReader::getLineType() {
  return buf[0];
}
char* GfaFileReader::getLine() {
  return buf;
}
unsigned long GfaFileReader::getLineNum() {
  return line_num;
}
GfaColumn GfaFileReader::getField(unsigned long idx) {
  assert(idx < field_count);
  return columns[idx];
}

char* GfaFileReader::getFieldContent(unsigned long idx) {
  assert(idx < field_count);
  return buf + columns[idx].offset;  
}
unsigned long GfaFileReader::getFieldLength(unsigned long idx) {
  assert(idx < field_count);
  return columns[idx].length;
}

void GfaFileReader::parseWord(long *ptr, unsigned long idx, char *str, unsigned long len) {
  if (!isValidWord(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": integer expected, but found '" << string(str,len) << "'.";
  }

  /* special handling for 0, because 0 is also error value of atol */
  if (len == 1 && str[0]=='0') {
    *ptr = 0;
  } else {
    *ptr = atol(str);
    assert(ptr != 0);
  }
}

void GfaFileReader::parseUword(unsigned long *ptr, unsigned long idx, char *str, unsigned long len) {
  if (!isValidUword(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": unsigned integer expected, but found '" << string(str,len) << "'.";
  }

  /* special handling for 0, because 0 is also error value of atol */
  if (len == 1 && str[0]=='0') {
    *ptr = 0;
  } else {
    *ptr = atol(str);
    assert(ptr != 0);
  }
}

void GfaFileReader::parsePos(GfaPos *ptr, unsigned long idx, char *str, unsigned long len) {
  if (!isValidPos(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": position expected, but found '" << string(str,len) << "'.";
  }

  ptr->val = atol(str);
  ptr->is_end = (str[len-1] == GFA_SENTINEL);
}

void GfaFileReader::parseLabel(string *ptr, unsigned long idx, char *str, unsigned long len) {
  if (!isValidLabel(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": Invalid label '" << string(str,len) << "'.";
  }
  ptr->assign(str, len);
}

void GfaFileReader::parseSegmentLabel(string *ptr, unsigned long idx, char *str, unsigned long len) {
  if (!isValidSegmentLabel(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": Invalid label '" << string(str,len) << "'.";
  }
  ptr->assign(str, len);
}

void GfaFileReader::parseSequence(GfaSequence *ptr, unsigned long idx, char *str, unsigned long len) {
  if (!isValidSequence(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": Invalid sequence '" << string(str,len) << "'.";
  }
  ptr->createFromString(str,len);
}

void GfaFileReader::parseOrientation(bool *ptr, unsigned long idx, char *str, unsigned long len) {
  if (!isValidOrientation(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": orientation (+/-) expected, but found '" << string(str,len) << "'.";
  }
  *ptr = (str[0] == GFA_REVERSE);
}

void GfaFileReader::parseAlignment(GfaAlignment *ptr, unsigned long idx, char *str, unsigned long len) {
  return ptr->createFromCigarString(str,len);
}

void GfaFileReader::parseRef(GfaRef *ptr, unsigned long idx, char *str, unsigned long len) {
  if (str[len-1] != '+' && str[len-1] != '-') {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": No orientation found in reference '" << string(str,len) << "'.";
  }
  if (!isValidLabel(str, len - 1)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": Invalid label found in reference '" << string(str,len) << "'.";
  }

  ptr->name.assign(str, len-1);
  ptr->is_reverse = (str[len-1] == GFA_REVERSE);
}

void GfaFileReader::parseReflist(vector<GfaRef> *ptr, unsigned long idx, char *str, unsigned long len, char separator) {
  unsigned long s_idx = 0, last_idx = 0;
  int had_err = 0;

  for (s_idx = 0; s_idx < len + 1 && had_err == 0; s_idx++) {
    if ((s_idx == len || str[s_idx] == separator) &&
        (str[s_idx-1] == GFA_FORWARD || str[s_idx-1] == GFA_REVERSE)) {
      if (s_idx < 2 || (s_idx - last_idx) < 2) {
        had_err = -1;
        break;
      }
      GfaRef ref;
      parseRef(&ref, idx, str+last_idx, s_idx-last_idx);
      ptr->push_back(ref);
      last_idx = s_idx + 1;
    } else if (s_idx == len || str[s_idx] < '!' || str[s_idx] > '~') {
      had_err = -1;
      break;
    }
  }

  if (had_err != 0) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": badly formatted list of references.";
  }
}
void GfaFileReader::parseReflistUnordered(vector<GfaRef> *ptr, unsigned long idx, char *str, unsigned long len) {
  unsigned long s_idx = 0, last_idx = 0;
  int had_err = 0;

  for (s_idx = 0; s_idx < len + 1 && had_err == 0; s_idx++) {
    if (s_idx == len || str[s_idx] == ' ') {
      if (s_idx < 1 || (s_idx - last_idx) < 1) {
        had_err = -1;
        break;
      }
      GfaRef ref;
      parseLabel(&ref.name, idx, str+last_idx, s_idx-last_idx);
      ptr->push_back(ref);
      last_idx = s_idx + 1;
    } else if (str[s_idx] < '!' || str[s_idx] > '~') {
      had_err = -1;
      break;
    }
  }
  if (had_err != 0) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": badly formatted list of references.";
  }
}

void GfaFileReader::parseAlignmentlist(vector<GfaAlignment> *ptr, unsigned long idx, char *str, unsigned long len) {
  unsigned long s_idx = 0, last_idx = 0;
  int had_err = 0;

  for (s_idx = 0; s_idx <= len && had_err == 0; s_idx++) {
    if (s_idx == len || str[s_idx] == ',') {
      if (s_idx < 1 || (s_idx - last_idx) < 1) {
        had_err = -1;
        break;
      }
      GfaAlignment al;
      parseAlignment(&al, idx, str+last_idx, s_idx-last_idx);
      ptr->push_back(al);
      last_idx = s_idx + 1;
    }
  }

  if (had_err != 0) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": badly formatted list of alignments.";
  }
}

void GfaFileReader::parseVariance(GfaVariance *ptr, unsigned long idx, char *str, unsigned long len) {
  if (len == 1 && str[0] == '*')
    return;
  if (!isValidUword(str, len)) {
    throw fatal_error() << "Line " << getLineNum() << ", column " << idx << ": expected variance (either '*' or an integer), but found '" << string(str,len) << "'.";
  }
  ptr->set(atol(str));
}


void GfaFileReader::parseGeneric(GfaLine* line, const char *fmt_string, ...) {
  unsigned long fmt_idx = 0;
  unsigned long col_idx = 1;
  va_list ap;
  va_start(ap, fmt_string);

  while (fmt_string[fmt_idx] != '\0') {
    if (col_idx >= fieldCount()) {
      throw fatal_error() << "Line " << getLineNum() << ": Not enough columns found.";
    }

    void *next_ptr = va_arg(ap, void*);
    if (next_ptr == NULL) {
      assert("Not enough arguments given" && false);
    }
  
    GfaColumn col = getField(col_idx);
    char *content = buf + col.offset;
    switch (fmt_string[fmt_idx]) {
      case GFA_FMT_WORD: {
        long *ptr = (long*)next_ptr;
        parseWord(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_UWORD: {
        unsigned long *ptr = (unsigned long*)next_ptr;
        parseUword(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_POS: {
        GfaPos *ptr = (GfaPos*)next_ptr;
        parsePos(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_LABEL: {
        string *ptr = (string *)next_ptr;
        parseLabel(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_SEGMENT_LABEL: {
        string *ptr = (string *)next_ptr;
        parseSegmentLabel(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_SEQUENCE: {
        GfaSequence *ptr = (GfaSequence*)next_ptr;
        parseSequence(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_ORIENTATION: {
        bool *ptr = (bool*)next_ptr;
        parseOrientation(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_ALIGNMENT: {
        GfaAlignment *ptr = (GfaAlignment*)next_ptr;
        parseAlignment(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_REF: {
        GfaRef *ptr = (GfaRef*)next_ptr;
        parseRef(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_REF_LIST: {
        vector<GfaRef> *ptr = (vector<GfaRef>*)next_ptr;
        parseReflist(ptr, col_idx, content, col.length, ' ');
        break;
      }
      case GFA_FMT_REF_LIST_V1: {
        vector<GfaRef> *ptr = (vector<GfaRef>*)next_ptr;
        parseReflist(ptr, col_idx, content, col.length, ',');
        break;
      }
      case GFA_FMT_REF_LIST_UNORDERED: {
        vector<GfaRef> *ptr = (vector<GfaRef>*)next_ptr;
        parseReflistUnordered(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_ALIGNMENT_LIST: {
        vector<GfaAlignment> *ptr = (vector<GfaAlignment>*)next_ptr;
        parseAlignmentlist(ptr, col_idx, content, col.length);
        break;
      }
      case GFA_FMT_VARIANCE: {
        GfaVariance *ptr = (GfaVariance*)next_ptr;
        parseVariance(ptr, col_idx, content, col.length);
        break;
      }
      default: {
        assert("Unknown character in format string!" && false);
      }
    }
    fmt_idx++;
    col_idx++;
  }
  for (; col_idx < fieldCount(); col_idx++) {
    GfaColumn col = getField(col_idx);
    line->addTag(new GfaTag(buf + col.offset, col.length));
  }
}

