#include "DataSet_RemLog.h"
#include "CpptrajStdio.h"

DataSet_RemLog::DataSet_RemLog() :
  // 0 dim indicates DataSet-specific write 
  DataSet(REMLOG, GENERIC, TextFormat(TextFormat::DOUBLE, 10, 4), 0)
{}

void DataSet_RemLog::AllocateReplicas(int n_replicas, GdimArray const& gdimIn,
                                      RepInfoArray const& repInfoIn,
                                      ReplicaDimArray const& repDimIn)
{
  ensemble_.clear();
  ensemble_.resize( n_replicas );
  groupDims_ = gdimIn;
  repInfo_ = repInfoIn;
  repDims_ = repDimIn;
}

int DataSet_RemLog::NumExchange() const {
  if (ensemble_.empty())
    return 0;
  else // Each member of the ensemble should have same # exchanges.
    return (int)ensemble_[0].size();
}

bool DataSet_RemLog::ValidEnsemble() const {
  ReplicaEnsemble::const_iterator member = ensemble_.begin();
  size_t first_size = (*member).size();
  for (; member != ensemble_.end(); ++member) {
    if ((*member).size() != first_size) {
      mprinterr("Error: In remlog data set %s size of ensemble member %zu (%zu) !="
                " size of first member (%zu)\n", Meta().Name().c_str(), // TODO: Change to legend
                member - ensemble_.begin() + 1, (*member).size(), first_size);
      return false;
    }
  }
  return true;
}

void DataSet_RemLog::TrimLastExchange() {
  if (ensemble_.empty()) return;
  ReplicaEnsemble::iterator member = ensemble_.begin();
  size_t min_size = (*member).size();
  ++member;
  for (; member != ensemble_.end(); ++member) {
    if ((*member).size() < min_size) min_size = (*member).size();
  }
  // Resize all member arrays to minimum
  for (member = ensemble_.begin(); member != ensemble_.end(); ++member)
    (*member).resize( min_size );
}

void DataSet_RemLog::Info() const {
  mprintf(" (%zu replicas, %zu dims)", ensemble_.size(), groupDims_.size());
} 
