#pragma once
#include "gfa/defines.h"
#include "gfa/line.h"
#include "gfa/error.h"
#include "gfa/filereader.h"
#include "gfa/types.h"
#include "gfa/alignment.h"

class GfaGraph;

class GfaGroup: public GfaReferenceableLine {
  public:
    GfaGroup();
    virtual ~GfaGroup();
    void fromLine(GfaFileReader* fr);
    friend ostream& operator<< (ostream &out, const GfaGroup &g);
    void print(ostream &out, GfaVersion _version) const;
    void resolve(GfaGraph* _g);
    void deleteme();
    char getTypeOriginal() const;
    const vector<GfaLine*>& getMembers();
    
  private:
    char type_original; //P or U or O
    char resolve_status;
    vector<GfaRef> refs;
    vector<GfaAlignment> alignments; 
    vector<GfaLine*> members;

    void resolveMembers(size_t idx);
    const GfaRef& getFirstRef() const;
    const GfaRef& getLastRef() const;
};
