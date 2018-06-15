#ifndef INC_ANALYSIS_TWOPARTICLEDIFFUSION_H
#define INC_ANALYSIS_TWOPARTICLEDIFFUSION_H
#include "Analysis.h"
/// <Enter description of Analysis_TwoParticleDiffusion here>
class Analysis_TwoParticleDiffusion : public Analysis {
  public:
    Analysis_TwoParticleDiffusion() {}
    DispatchObject* Alloc() const { return (DispatchObject*)new Analysis_TwoParticleDiffusion(); }
    void Help() const;

    Analysis::RetType Setup(ArgList&, AnalysisSetup&, int);
    Analysis::RetType Analyze();
  private:
    DataSet_Coords* coords_;
    AtomMask mask_;
    DataSet* out_;
    int maxlag_;
};
#endif
