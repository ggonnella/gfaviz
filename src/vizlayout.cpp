#include "vizlayout.h"
#include "vizgraph.h"

VizLayout::VizLayout(VizGraph* _vg) {
  vg=_vg;
  widget = NULL;
}

void VizLayout::exec(double ratio) {
  VizComponentSplitterLayout compLayouter;
  compLayouter.setRatio(ratio);
  compLayouter.setLayoutModule(getLayoutModule());
  GraphAttributes GA = vg->GA;
  compLayouter.call(GA);
  vg->GA = GA;
  vg->draw();
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
  SM->setIterations(50);
}

void VizLayoutSM::apply(double ratio) {
  reset();
  exec(ratio);
}

void VizLayoutSM::applyFromGUI(double ratio) {
  reset();
  SM->setIterations(optionsForm.Iterations->value());
  exec(ratio);
}

void VizLayoutFMMM::reset() {
  if (FMMM) {
    //TODO:
    //delete FMMM;
    FMMM = NULL;
  }
  FMMM = new FMMMLayout();
  //FMMM->randSeed(rand());
  FMMM->useHighLevelOptions(true);
  FMMM->newInitialPlacement(true);
}

void VizLayoutFMMM::apply(double ratio) {
  reset();
  exec(ratio);
}

void VizLayoutFMMM::applyFromGUI(double ratio) {
  reset();
  FMMM->fixedIterations(optionsForm.FixedIterations->value());
  FMMM->fineTuningIterations(optionsForm.FineIterations->value());
  FMMM->qualityVersusSpeed((FMMMOptions::QualityVsSpeed)optionsForm.QvS->currentData().toInt());
  exec(ratio);
}
