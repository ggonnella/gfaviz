#pragma once

class VizGraph;

class VizLayout {
  public:
    VizLayout();
    
    virtual void exec(VizGraph *graph) {};
    
  protected:
    virtual void prepare(VizGraph *graph) {};
};

class VizLayoutSM {
  public:
    VizLayoutSM();
    virtual void exec(VizGraph *graph);
  
  protected:
    virtual void prepare(VizGraph *graph);
};

class VizLayoutFMMM {
  public:
    VizLayoutFMMM();
    virtual void exec(VizGraph *graph);
    
  protected:
    virtual void prepare(VizGraph *graph);
};
