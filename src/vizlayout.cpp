#include "vizlayout.h"
#include "vizgraph.h"

VizLayout::VizLayout(VizGraph* _vg) {
  vg=_vg;
  useEdgelengths = false;
  widget = NULL;
}

void VizLayout::exec(double ratio) {
  VizComponentSplitterLayout compLayouter;
  compLayouter.setRatio(ratio);
  compLayouter.setLayoutModule(getLayoutModule());
  GraphAttributes GA = vg->GA;
  connect(&compLayouter, &VizLayoutModule::progress, this, &VizLayout::progress, Qt::UniqueConnection);
  emit progress(0);
  if (useEdgelengths)
    compLayouter.call(GA,vg->edgeLengths);
  else
    compLayouter.call(GA);
  vg->GA = GA;
  vg->draw();
}
void VizLayout::abort() {
  
}

void VizLayoutSM::reset() {
  if (SM) {
    //TODO:
    //delete SM;
    SM = NULL;
  }
  SM = new VizStressMinimization();
  SM->useEdgeCostsAttribute(true);
  SM->layoutComponentsSeparately(true);
  SM->setIterations(100);
}

void VizLayoutSM::apply(double ratio, bool fromGui) {
  reset();
  if (fromGui)
    SM->setIterations(optionsForm.Iterations->value());
  exec(ratio);
}

void VizLayoutFMMM::reset() {
  if (FMMM) {
    //TODO:
    //delete FMMM;
    FMMM = NULL;
  }
  FMMM = new VizFMMMLayout();
  //FMMM->randSeed(rand());
  FMMM->useHighLevelOptions(true);
  FMMM->newInitialPlacement(true);
}

void VizLayoutFMMM::apply(double ratio, bool fromGui) {
  reset();
  if (fromGui) {
    FMMM->fixedIterations(optionsForm.FixedIterations->value());
    FMMM->fineTuningIterations(optionsForm.FineIterations->value());
  }
  exec(ratio);
}
