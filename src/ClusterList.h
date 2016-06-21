#ifndef INC_CLUSTERLIST_H
#define INC_CLUSTERLIST_H
#include <list>
#include "ArgList.h"
#include "ClusterNode.h"
#include "ClusterMatrix.h"
// Class: ClusterList
/** This base class holds all the individual clusters, as well as routines 
  * that can be used to obtain information on clusters after clustering.
  */
class ClusterList {
  public:
    enum DistMetricType { RMS = 0, DME, SRMSD, DATA };
    static const char* MetricString( DistMetricType );
    ClusterList();
    virtual ~ClusterList();
    int Nclusters()                  const { return (int)clusters_.size(); }

    void SetDebug(int);
    void Renumber(bool);
    void Summary(std::string const&,int) const;
    void Summary_Part(std::string const&,int,std::vector<int> const&) const;
    void PrintClustersToFile(std::string const&,int);
    void PrintClusters() const;
#   ifdef MPI
    void SetComm(Parallel::Comm const& commIn) { comm_ = commIn; }
#   endif
    /// Set up appropriate cluster distance calculation
    int SetupCdist( ClusterDist::DsArray const&, DistMetricType, bool, bool, std::string const&);
    /// Calculate distances between frames if necessary.
    int CalcFrameDistances(DataSet*, ClusterDist::DsArray const&, int, int);
    // ----- Inherited by individual clustering methods ----
    virtual int SetupCluster(ArgList&) = 0;
    virtual void ClusteringInfo() const = 0;
    virtual int Cluster() = 0;
#   ifdef TIMER
    virtual void Timing(double) const = 0;
#   endif
#   ifdef MPI
    virtual int SyncClusters() { return 1; } // TODO pure virtual
#   endif
    /// Iterator over clusters
    typedef std::list<ClusterNode>::iterator cluster_it;
    cluster_it begin() { return clusters_.begin(); }
    cluster_it end()   { return clusters_.end();   }
    /// Const Iterator over clusters
    typedef std::list<ClusterNode>::const_iterator cluster_iterator;
    const cluster_iterator begincluster() const { return clusters_.begin(); }
    const cluster_iterator endcluster()   const { return clusters_.end();   }
    /// Remove clusters with no members.
    void RemoveEmptyClusters();
    /// Calculate distances between each cluster
    void CalcClusterDistances();
    /// Calculate cluster silhouettes
    void CalcSilhouette(std::string const&) const;

    void DrawGraph(bool,DataSet*,double,int) const;
  protected:
    virtual void AddSievedFrames() = 0;
    virtual void ClusterResults(CpptrajFile&) const = 0;
#   ifdef MPI
    int CheckClusterComm() const;
    Parallel::Comm const& ClusterComm() const { return comm_; }
#   endif
    void AddSievedFramesByCentroid();
    DataSet_Cmatrix const& FrameDistances() const { return *frameDistances_; }
    int debug_;
    /// Store individual cluster info; frame numbers, centroid, etc.
    std::list<ClusterNode> clusters_;
    /// Distances between each cluster.
    ClusterMatrix ClusterDistances_;
    /// Used to calculate distances between frames and/or centroids.
    ClusterDist* Cdist_;
    /// Add specified frames to a new cluster.
    int AddCluster(ClusterDist::Cframes const&);
  private:
    static const char* XMGRACE_COLOR[];
    /// Determine max name width
    unsigned int DetermineNameWidth() const;
    /// Calculate the Davies-Bouldin index of clusters.
    double ComputeDBI(CpptrajFile&);
    /// Calculate pseudo-F statistic.
    double ComputePseudoF(CpptrajFile&);

    /// Hold pointer to matrix containing distances between each frame.
    DataSet_Cmatrix* frameDistances_;
#   ifdef MPI
    Parallel::Comm comm_;
#   endif
};
#endif
