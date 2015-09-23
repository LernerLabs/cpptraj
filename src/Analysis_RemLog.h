#ifndef INC_ANALYSIS_REMLOG_H
#define INC_ANALYSIS_REMLOG_H
#include "Analysis.h"
#include "DataSet_RemLog.h"
#include "DataSet_integer.h"
class Analysis_RemLog : public Analysis {
  public:
    Analysis_RemLog();
    static DispatchObject* Alloc() { return (DispatchObject*)new Analysis_RemLog(); }
    static void Help();
    Analysis::RetType Setup(ArgList&,DataSetList*,TopologyList*,DataFileList*,int);
    Analysis::RetType Analyze();
  private:
    enum ModeType { NONE = 0, CRDIDX, REPIDX };

    typedef std::vector<int> Iarray;
    typedef std::vector<DataSet_integer> DSI_array;
    /// Track exchange stats for each dimension.
    class RepStats {
      public:
        RepStats(int nreps) :
          acceptUp_(nreps, 0), acceptDown_(nreps, 0), attempts_(0) {}

        Iarray acceptUp_;     ///< # exchanges to the right accepted for each replica.
        Iarray acceptDown_;   ///< # exchanges to the left accepted for each replica.
        int attempts_;        ///< Total # exchanges in dimension.
    };
    /// Trip status
    enum RepStatusType { UNKNOWN = 0, HIT_BOTTOM, HIT_TOP };
    /// Track trip stats for each dimension
    class TripStats {
      public:
        TripStats(int nreps) :
          status_(nreps, UNKNOWN), bottom_(nreps, 0), roundTrip_(nreps) {}

        Iarray status_;       ///< Current status of each crdidx.
        Iarray bottom_;       ///< Frame at which each crdidx hit bottom.
        DSI_array roundTrip_; ///< Array of round trip times for each crdidx.
    };

    int debug_;
    bool calculateStats_;
    bool calculateLifetimes_;
    bool printIndividualTrips_;
    DataSet_RemLog* remlog_;
    DataSet* repTimeMatrix_;
    ModeType mode_;
    std::vector<DataSet*> outputDsets_;
    CpptrajFile* lifetimes_;
    CpptrajFile* statsout_;
    CpptrajFile* reptime_;
    CpptrajFile* acceptout_;
    int calcRepFracSlope_;
    CpptrajFile* repFracSlope_;
};
#endif
