#ifndef INC_SYMMETRICRMSDCALC_H
#define INC_SYMMETRICRMSDCALC_H
#include "Topology.h"
#include "Hungarian.h"
#include "AtomMap.h"
/// Class for performing symmetry-corrected RMSD calculations.
class SymmetricRmsdCalc {
  public:
    SymmetricRmsdCalc();
    SymmetricRmsdCalc(AtomMask const&, bool, bool, Topology const&);
    /// Set target mask string, fit, and mass options.
    int InitSymmRMSD(std::string const&, bool, bool, bool, int);
    /// Setup target mask, find symmetric atoms.
    int SetupSymmRMSD(Topology const&);
    /// Calculate symm. RMSD using target and reference that already correspond to tgtMask
    double SymmRMSD(Frame const&, Frame&);
    /// Calculate symm. RMSD of target to centered ref with potential coordinate remapping.
    double SymmRMSD_TGT(Frame const&, Frame const&);
    /// Calculate symm. RMSD using pre-centered reference corresponding to tgtMask.
    double SymmRMSD_CenteredRef(Frame const&, Frame const&);
    AtomMask const& TgtMask()     const { return tgtMask_;     }
    const Frame* RemapFrame()     const { return &remapFrame_; }
    bool Fit()                    const { return fit_;         }
    bool UseMass()                const { return useMass_;     }
    bool ReMap()                  const { return remap_;       }
    Matrix_3x3 const& RotMatrix() const { return rotMatrix_;   }
    Vec3 const& TgtTrans()        const { return tgtTrans_;    }
  private:
    enum atomStatusType { UNSELECTED = 0, NONSYMM, SYMM };
    typedef std::vector<int> Iarray;
    typedef std::vector<Iarray> AtomIndexArray;
    
    void FindSymmetricAtoms(int, AtomMap const&, std::string const&, Iarray&, Iarray&) const;

    /// Array of groups of potentially symmetric atoms
    AtomIndexArray SymmetricAtomIndices_;
    int debug_;
    Hungarian cost_matrix_; ///< Hungarian algorithm cost matrix.
    Iarray AMap_;           ///< AMap_[oldSelectedTgt] = newSelectedTgt
    Iarray targetMap_;      ///< targetMap_[oldTgt] = newTgt
    Frame remapFrame_;      ///< Target frame re-mapped for symmetry
    Frame tgtRemap_;        ///< Selected target atoms re-mapped for symmetry.
    Frame selectedTgt_;     ///< Selected atoms from target frame.
    AtomMask tgtMask_;      ///< Mask selecting atoms in target for RMSD calc.
    Matrix_3x3 rotMatrix_;  ///< Hold best-fit rotation matrix for target.
    Vec3 tgtTrans_;         ///< Hold translation of target to origin.
    bool fit_;              ///< If true, perform RMS best-fit.
    bool useMass_;          ///< If true, mass-weight calc.
    bool remap_;            ///< If true, remap target frame for symmetry.
};
#endif