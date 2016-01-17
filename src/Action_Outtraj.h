#ifndef INC_ACTION_OUTTRAJ_H
#define INC_ACTION_OUTTRAJ_H
#include "Action.h"
#include "Trajout_Single.h"
#include "DataSet_1D.h"
/// Write out a trajectory inside the ActionList
class Action_Outtraj: public Action {
  public:
    Action_Outtraj() : outtraj_(0), associatedParm_(0), isSetup_(false), isActive_(true) {}
    DispatchObject* Alloc() const { return (DispatchObject*)new Action_Outtraj(); }
    void Help() const;
  private:
    Action::RetType Init(ArgList&, ActionInit&, int);
    Action::RetType Setup(ActionSetup&);
    Action::RetType DoAction(int, ActionFrame&);
    void Print();
#   ifdef MPI
    int ParallelActionInit(Parallel::Comm const&);
    int SyncAction(Parallel::Comm const&);
#   endif
    Trajout_Single* outtraj_;
    Topology* associatedParm_;
    bool isSetup_;
    bool isActive_;
    std::vector<double> Max_;
    std::vector<double> Min_;
    std::vector<DataSet_1D*> Dsets_;
#   ifdef MPI
    Parallel::Comm trajComm_;
    int total_frames_;
#   endif
};
#endif
