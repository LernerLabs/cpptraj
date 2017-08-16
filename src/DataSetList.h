#ifndef INC_DATASETLIST_H
#define INC_DATASETLIST_H
#include <vector>
#include "DataSet.h"
#include "ArgList.h" // GetReferenceFrame, GetTopology
#include "ReferenceFrame.h" // GetReferenceFrame
#ifdef TIMER
# include "Timer.h"
#endif
/// Hold list of DataSets.
/** Main class for handling DataSets. All DataSet types can be allocated 
  * by DataSetList. There is a master DataSetList in CpptrajState that will
  * hold all data (generated by Actions, read in from file, etc) so it is 
  * available for subsequent use.
  * There are currently two special classes of DataSet which have extra
  * functions in DataSetList: TOPOLOGY and REF_COORDS (see below).
  * This class can also be used to hold copies of DataSets, i.e. it will 
  * hold pointers to the DataSet classes but will not be responsible for
  * allocating/freeing memory. This is made use of by e.g. the 
  * GetMultipleSets() function and in the DataFile class.
  */
class DataSetList {
  public:
    typedef std::vector<DataSet*> DataListType;
    typedef std::vector<double> Darray;

    DataSetList();
    ~DataSetList();

    DataSetList& operator+=(DataSetList const&);
    /// \return DataSet at didx.
    DataSet* operator[](int didx) const { return DataList_[didx]; } // FIXME: No bounds check
    /// DataSetList default iterator
    typedef DataListType::const_iterator const_iterator;
    /// Iterator to beginning of dataset list
    const_iterator begin() const { return DataList_.begin(); }
    /// Iterator to end of dataset list
    const_iterator end()   const { return DataList_.end();   }
    /// Clear all non-Topology and non-Reference DataSets
    void Clear();
    /// Sort sets in the DataSetList
    void Sort();
    /// True if no DataSets in list.
    bool empty()           const { return DataList_.empty(); }
    /// \return number of datasets in the list 
    size_t size()          const { return DataList_.size();  }
    /// \return Number of frames from last call to AllocateSets().
    long int MaxFrames()   const { return maxFrames_;        }
    /// Set current ensemble number.
    void SetEnsembleNum(int i)   { ensembleNum_ = i;         }
    /// Set DataSetList and underlying DataSet debug level
    void SetDebug(int d) { debug_ = d; }
    /// Set DataSets pending status.
    void SetDataSetsPending(bool b) { dataSetsPending_ = b; }
    /// Make all sets not part of an ensemble part of given ensemble.
    //void MakeDataSetsEnsemble(int);
    /// \return Ensemble number; -1 if not an ensemble
    int EnsembleNum()      const { return ensembleNum_;      }
    /// \return True if Actions have indicated DataSets will be generated.
    bool DataSetsPending() const { return dataSetsPending_;  }
    /// Allocate 1D DataSet memory based on current max# expected frames.
    void AllocateSets(long int);
    /// Set width and precision of specified DataSets in the list.
    void SetPrecisionOfDataSets(std::string const&, int, int);

    /// Remove set from the list.
    void RemoveSet( DataSet* );
    /// Remove set from list but do not destroy.
    DataSet* PopSet( DataSet* );

    /// Get DataSet matching specified attributes exactly.
    DataSet* CheckForSet( MetaData const& ) const;
    /// Get DataSet corresponding to specified argument.
    DataSet* GetDataSet( std::string const& ) const;
    /// Get multiple DataSets matching specified argument.
    DataSetList GetMultipleSets( std::string const& ) const;
    /// Get all DataSets matching specified argument and type.
    DataSetList GetSetsOfType( std::string const&, DataSet::DataType ) const;
    /// Select multiple sets, no warning if none found.
    DataSetList SelectSets( std::string const& ) const;
    /// Select multiple sets by type.
    DataSetList SelectSets( std::string const&, DataSet::DataType ) const;
    /// Select multiple sets by group.
    DataSetList SelectGroupSets( std::string const&, DataSet::DataGroup ) const;
    /// Find next set of specified type with given name.
    DataSet* FindSetOfType(std::string const&, DataSet::DataType) const;
    /// Find COORDS DataSet or create default COORDS DataSet.
    DataSet* FindCoordsSet(std::string const&);

    /// Generate name based on given default and # of DataSets.
    std::string GenerateDefaultName(std::string const&) const;
    /// Allocate DataSet of given type - do not add to list.
    static DataSet* Allocate(DataSet::DataType);
    /// Add DataSet to list; set up default name if no name specified.
    DataSet* AddSet( DataSet::DataType, MetaData const&, const char*);
    /// Add DataSet to list with given MetaData.
    DataSet* AddSet( DataSet::DataType, MetaData const&);
    /// Add DataSet, no check for already existing set.
    DataSet* AddSet_NoCheck(DataSet::DataType, MetaData const&);
    /// Add an already set up DataSet to list; memory for DataSet will be freed.
    int AddSet( DataSet* );
    /// Add new sets or append to existing ones.
    int AddOrAppendSets(std::string const&, Darray const&, DataListType const&);
    /// Add a copy of the DataSet to the list; memory for DataSet will not be freed.
    void AddCopyOfSet(DataSet*);
    /// Print info on DataSets in the list
    void List() const;
    /// List all non-Topology/Reference data sets.
    void ListDataOnly() const;
#   ifdef MPI
    /// Indicate whether sets added to the list need to be synced
    void SetNewSetsNeedSync(bool b) { newSetsNeedSync_ = b; }
    /// Call sync for DataSets in the list (MPI only)
    int SynchronizeData(Parallel::Comm const&);
#   endif

    // REF_COORDS functions ----------------------
    /// reference arg help text
    static const char* RefArgs;
    /// Get reference frame DataSet from args
    ReferenceFrame GetReferenceFrame(ArgList&) const;
    /// Set active reference for distance-based masks.
    int SetActiveReference( ArgList& );
    /// List all reference frames.
    void ListReferenceFrames() const;
    /// Remove all reference frames.
    void ClearRef();
    // TOPOLOGY functions ------------------------
    /// GetTopology arg help text
    static const char* TopArgs;
    /// GetTopByIndex arg help text.
    static const char* TopIdxArgs;
    /// Get topology from args
    Topology* GetTopology(ArgList&) const;
    /// Get topology from args; allow index specified without 'parmindex'
    Topology* GetTopByIndex(ArgList&) const;
    /// List all topologies
    void ListTopologies() const;
    /// Remove all topologies.
    void ClearTop();
#   ifdef TIMER
    void Timing() const;
#   endif
  private:
    /// Clear entire DataSetList
    void ClearAll();
    /// Search for and remove specified data set if found, optionally free memory.
    DataSet* EraseSet( DataSet*, bool );
    /// Warn if DataSet not found but may be pending.
    inline void PendingWarning() const;
    /// Wrapper around DataList_.push_back() that does extra bookkeeping.
    void Push_Back(DataSet*);
    /// Internal print routine
    static inline void PrintList(DataListType const&);
    /// Get reference using keywords; set error integer if error occurs.
    DataSet* GetReferenceSet(ArgList&, int&) const;
    /// Set specified reference data set as the active reference.
    int SetActiveReference(DataSet*);
    /// Get topology using keywords
    DataSet* GetTopByKeyword(ArgList&, int&) const;
#   ifdef TIMER
    Timer time_total_;
    Timer time_check_;
    Timer time_setup_;
    Timer time_push_;
#   endif
    DataSet* activeRef_;    ///< Current active reference for distance-based masks.
    long int maxFrames_;    ///< Hold number of frames from most recent AllocateSets() call.
    int debug_;             ///< DataSet debug level
    int ensembleNum_;       ///< Ensemble member number
    bool hasCopies_;        ///< True if DataSets should not be freed.
    bool dataSetsPending_;  ///< True if Actions will generate DataSets in the future.
    DataListType DataList_; ///< List of DataSets
    DataListType RefList_;  ///< Pointers to reference data sets.
    DataListType TopList_;  ///< Pointers to topology data sets.
    /// Hold descriptions and allocators for all DataSet types.
    struct DataToken {
      const char* Description;
      DataSet::AllocatorType Alloc;
    };
    static const DataToken DataArray[];
    typedef const DataToken* TokenPtr;
#   ifdef MPI
    bool newSetsNeedSync_; ///< If true, any sets added need to be synced.
#   endif
};
#endif
