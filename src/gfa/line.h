#pragma once
#include "gfa/defines.h"
#include "gfa/tag.h"
#include <vector>

class GfaGraph;
class GfaGroup;

class GfaLine
{
  public:
    GfaLine(GfaLinetype _t);
    ~GfaLine();
    
    GfaLinetype getType() const;
    void setType(GfaLinetype _t);
    
    const string& getID();
    const string& getName() const;
    void setName(string _n);
    bool hasName() const;
    void setGraph(GfaGraph* _g);
    GfaVersion getVersion() const;
    void setVersion(GfaVersion _v);
    
    void addTag(GfaTag* tag);
    void removeTag(const char key[2], GfaTagType tagtype = GFA_TAG_WILDCARD);
    bool hasTag(const char key[2], GfaTagType tagtype = GFA_TAG_WILDCARD) const;
    GfaTag* getTag(const char key[2], GfaTagType tagtype = GFA_TAG_WILDCARD) const;
    const vector<GfaTag*>& getTags() const;
    
    bool isFilled();
    void setFilled();
    bool isAdded();
    void setAdded();
    
    friend ostream& operator<< (ostream &out, const GfaLine &line);
    virtual void print(ostream &out,GfaVersion version) const; //{}
    void remove();
    virtual void deleteme() {}; //Should not be called by users
    
    //virtual void removeWithoutDependencies() {};
    //virtual void addDependenciesToSet(GfaLineSet* set) {};
    
  protected:
    GfaLinetype type;
    GfaGraph* graph;
    GfaVersion version;
    vector<GfaTag*> tags;
    
    string getIDGeneric() const;
    bool is_named;
    bool id_set;
    bool is_filled;
    bool is_added;
    string name;
    
    void printTags(ostream &out) const;
    
};

class GfaReferenceableLine : public GfaLine {
  public:
    GfaReferenceableLine(GfaLinetype _t) : GfaLine(_t) {};
    void addGroup(GfaGroup *group);
    void removeGroup(GfaGroup *group);
    const vector<GfaGroup*>& getGroups() const;
    
  protected:
    vector<GfaGroup*> groups;
};
