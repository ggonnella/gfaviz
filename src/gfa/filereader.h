#pragma once
#include <fstream>
#include <vector>
#include <utility>
#include <string>
#include "gfa/error.h"
#include "gfa/types.h"
#include "gfa/alignment.h"
#include "gfa/sequence.h"
#include <stdarg.h>


using namespace std;
class GfaGraph;

struct GfaColumn {
  unsigned long offset;
  unsigned long length;
};

class GfaFileReader {
  public:
    GfaFileReader();
    ~GfaFileReader();  
    
    void open(const char *filename);
    bool readNextLine();
    char getLineType();
    char* getLine();
    unsigned long fieldCount();
    unsigned long getLineNum();
    GfaColumn getField(unsigned long idx);
    char* getFieldContent(unsigned long idx);
    unsigned long getFieldLength(unsigned long idx);
    void parseGeneric(GfaLine* line, const char *fmt_string, ...);
    
    //static GfaGraph* graphFromFile(char *filename);
    
    
  private:
    FILE *fp;
    char *buf;
    unsigned long buf_pos;
    unsigned long buf_size;
    
    char *filebuf;
    unsigned long filebuf_size;
    unsigned long filebuf_pos;
    unsigned long filebuf_left;
    
    unsigned long field_count;
    unsigned long line_num;
    vector<GfaColumn> columns;
    
    char getNext();
    void parseWord(long *ptr, unsigned long idx, char *str, unsigned long len);
    void parseUword(unsigned long *ptr, unsigned long idx, char *str, unsigned long len);
    void parsePos(GfaPos *ptr, unsigned long idx, char *str, unsigned long len);
    void parseLabel(string *ptr, unsigned long idx, char *str, unsigned long len);
    void parseSegmentLabel(string *ptr, unsigned long idx, char *str, unsigned long len);
    void parseSequence(GfaSequence *ptr, unsigned long idx, char *str, unsigned long len);
    void parseOrientation(bool *ptr, unsigned long idx, char *str, unsigned long len);
    void parseAlignment(GfaAlignment *ptr, unsigned long idx, char *str, unsigned long len);
    void parseRef(GfaRef *ptr, unsigned long idx, char *str, unsigned long len);
    void parseReflist(vector<GfaRef> *ptr, unsigned long idx, char *str, unsigned long len, char separator);
    void parseReflistUnordered(vector<GfaRef> *ptr, unsigned long idx, char *str, unsigned long len);
    void parseAlignmentlist(vector<GfaAlignment> *ptr, unsigned long idx, char *str, unsigned long len);
    void parseVariance(GfaVariance *ptr, unsigned long idx, char *str, unsigned long len);
};

typedef enum
{
  GFA_FMT_WORD = 'w', /* Word */
  GFA_FMT_UWORD = 'u', /* Unsigned word */
  GFA_FMT_POS = 'p', /* Position, unsigned integer with optional $ sentinel */
  GFA_FMT_LABEL = 'l', /* also called ID in the specification */
  GFA_FMT_SEGMENT_LABEL = 'L', /* more restrictive labels */
  GFA_FMT_REF = 'r', /* concatenation of label and orientation */
  GFA_FMT_REF_LIST = 'O',/* space separated list of ordered refs (with +/-) */
  GFA_FMT_REF_LIST_V1 = 'P',/* comma separated list of ordered refs (with +/-)*/
  GFA_FMT_REF_LIST_UNORDERED = 'U', /* space separated list of unordered refs
                                       (no +/-) */
  GFA_FMT_SEQUENCE = 's', /* any printable character excluding whitespace */
  GFA_FMT_ORIENTATION = 'o', /* true if reverse (-), false if not (+) */
  GFA_FMT_VARIANCE = 'v', /* (* | <var:int>) */
  GFA_FMT_ALIGNMENT = 'a', /* CIGAR String allowing MDIP (as of GFA2) */
  GFA_FMT_ALIGNMENT_LIST = 'A' /* comma-separated list of Alignments */
} GfaParseFormatString;
