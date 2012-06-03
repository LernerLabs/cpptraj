// Closest
// Find closest waters to atoms in mask.
#include <cmath>
#include <algorithm> // sort
#include <cfloat> // DBL_MAX
#ifdef _OPENMP
#  include "omp.h"
#endif
#include "Action_Closest.h"
#include "CpptrajStdio.h"
#include "ParmFile.h"

// CONSTRUCTOR
Closest::Closest() :
  outFile_(NULL),
  framedata_(NULL),
  moldata_(NULL),
  distdata_(NULL),
  atomdata_(NULL),
  Nclosest_(0),
  prefix_(NULL),
  closestWaters_(0),
  firstAtom_(false),
  newParm_(NULL),
  NsolventMolecules_(0)
//  oldParm_(NULL)
{
  //fprintf(stderr,"Closest Con\n");
  useImage_ = true;
} 

// DESTRUCTOR
Closest::~Closest() {
  //fprintf(stderr,"Closest Destructor.\n");
  if (newParm_!=NULL) delete newParm_;
}

// Closest::init()
/** Expected call: closest <# to keep> <mask> [noimage] [first/oxygen] 
  *                [closestout <filename> [outprefix <parmprefix>]
  */
int Closest::init( ) {
  // Get Keywords
  closestWaters_ = actionArgs.getNextInteger(-1);
  if (closestWaters_ < 0) {
    mprinterr("Error: Closest::init(): Invalid # solvent molecules to keep (%i).\n",
              closestWaters_);
    return 1;
  }
  if ( actionArgs.hasKey("oxygen") || actionArgs.hasKey("first") )
    firstAtom_=true;
  useImage_ = !(actionArgs.hasKey("noimage"));
  prefix_ = actionArgs.getKeyString("outprefix",NULL);
  // Setup output file and sets if requested.
  // Will keep track of Frame, Mol#, Distance, and first solvent atom
  char *filename = actionArgs.getKeyString("closestout",NULL);
  if (filename != NULL) {
    // Set up datasets
    framedata_ = outList_.Add(DataSet::INT,(char*)"Frame\0","Frame");
    moldata_   = outList_.Add(DataSet::INT,(char*)"Mol\0","Mol");
    distdata_  = outList_.Add(DataSet::DOUBLE,(char*)"Dist\0","Dist");
    atomdata_  = outList_.Add(DataSet::INT,(char*)"FirstAtm\0","FirstAtm");
    if (framedata_==NULL || moldata_==NULL || distdata_==NULL || atomdata_==NULL) {
      mprinterr("Error: Closest::init(): Could not setup data sets for output file %s\n",
                filename);
      return 1;
    }
    // Add sets to datafile in list.
    outFile_ = DFL->Add(filename, framedata_);
    outFile_ = DFL->Add(filename, moldata_);
    outFile_ = DFL->Add(filename, distdata_);
    outFile_ = DFL->Add(filename, atomdata_);
    if (outFile_==NULL) {
      mprintf("Error: Closest::init(): Could not setup output file %s\n",filename);
      return 1;
    }
  }

  // Get Masks
  char *mask1 = actionArgs.getNextMask();
  if (mask1==NULL) {
    mprinterr("Error: Closest::init(): No mask specified.\n");
    return 1;
  }
  distanceMask_.SetMaskString(mask1);

  mprintf("    CLOSEST: Finding closest %i solvent molecules to atoms in mask %s\n",
          closestWaters_, distanceMask_.MaskString());
  if (!useImage_) 
    mprintf("             Imaging will be turned off.\n");
  if (firstAtom_)
    mprintf("             Only first atom of solvent molecule used for distance calc.\n");
  if (outFile_!=NULL)
    mprintf("             Closest molecules will be saved to %s\n",outFile_->Filename());
  if (prefix_!=NULL)
    mprintf("             Stripped topology file will be written with prefix %s\n",prefix_);

  return 0;
}

// Closest::setup()
/** Like the strip action, closest will modify the current parm keeping info
  * for atoms in mask plus the closestWaters solvent molecules. Set up the
  * vector of MolDist objects, one for every solvent molecule in the original
  * parm file. Atom masks for each solvent molecule will be set up.
  */
int Closest::setup() {
  // If there are no solvent molecules this action is not valid.
  if (currentParm->Nsolvent()==0) {
    mprintf("Warning: Closest::setup: Parm %s does not contain solvent.\n",currentParm->c_str());
    return 1;
  }
  // If # solvent to keep >= solvent in this parm the action is not valid.
  // TODO: Just use max # waters?
  if (closestWaters_ >= currentParm->Nsolvent()) {
    mprintf("Warning: Closest::setup: # solvent to keep (%i) >= # solvent molecules in\n",
            closestWaters_);
    mprintf("                           %s (%i).\n",currentParm->c_str(),
            currentParm->Nsolvent());
    return 1;
  } 

  // LOOP OVER MOLECULES
  // 1: Check that all solvent molecules contain same # atoms. Solvent 
  //    molecules must be identical for the command to work properly; 
  //    the prmtop strip occurs only once so the solvent params become fixed.
  int NsolventAtoms = -1;
  // 2: Set up a mask for all solvent molecules.
  SolventMols_.clear();
  // NOTE: May not be necessary to init 'solvent'
  MolDist solvent;
  solvent.D = 0.0;
  solvent.mol = 0;
  SolventMols_.resize(currentParm->Nsolvent(), solvent);
  std::vector<MolDist>::iterator mdist = SolventMols_.begin();
  int molnum = 1;
  // 3: Set up the soluteMask for all non-solvent molecules.
  //soluteMask_.ResetMask();
  stripMask_.ResetMask();
  int newnatom = 0;
  int nclosest = 0;
  keptWaterAtomNum_.resize(closestWaters_);
  for (Topology::mol_iterator solvmol = currentParm->MolStart();
                              solvmol != currentParm->MolEnd(); ++solvmol)
  {
    // DEBUG:
    //if ( (*solvmol).IsSolvent() ) mprintf("\tMOL %i is SOLVENT.\n",molnum);
    //else mprintf("\tMOL %i is not solvent.\n", molnum);
    if ( !(*solvmol).IsSolvent() ) { // Not solvent, add to solute mask.
      //soluteMask_.AddAtomRange( (*solvmol).BeginAtom(), (*solvmol).EndAtom() );
      stripMask_.AddAtomRange( (*solvmol).BeginAtom(), (*solvmol).EndAtom() );
      newnatom += (*solvmol).NumAtoms();
    } else {                         // Solvent, check for same # of atoms.
      if (NsolventAtoms == -1)
        NsolventAtoms = (*solvmol).NumAtoms();
      else if ( NsolventAtoms != (*solvmol).NumAtoms() ) {
        mprinterr("Error: closest: Solvent molecules in %s are not of uniform size.\n",
                  currentParm->c_str());
        mprinterr("       First solvent mol = %i atoms, solvent mol %i = %i atoms.\n",
                  NsolventAtoms, molnum, (*solvmol).NumAtoms());
        return 1;
      }
      // NOTE: mol here is the output molecule number which is why it
      //       starts from 1.
      (*mdist).mol = molnum;
      (*mdist).mask.AddAtomRange( (*solvmol).BeginAtom(), (*solvmol).EndAtom() );
      // For solvent molecules that will be kept, record what the atom number
      // will be in the new stripped parm.
      if (nclosest < closestWaters_) {
        mprintf("CDBG: Mol %i old atom# %i, new atom# %i\n", molnum, (*solvmol).BeginAtom()+1, newnatom+1);
        keptWaterAtomNum_[nclosest] = newnatom;
        stripMask_.AddAtomRange( (*solvmol).BeginAtom(), (*solvmol).EndAtom() );
        newnatom += (*solvmol).NumAtoms();
        ++nclosest;
      }
      //SolventMols[solventMol].mask.PrintMaskAtoms("solvent");
      ++mdist;
    }
    ++molnum;
  }

  // Setup distance atom mask
  // NOTE: Should ensure that no solvent atoms are selected!
  if ( currentParm->SetupIntegerMask(distanceMask_) ) return 1;
  if (distanceMask_.None()) {
    mprintf("Warning: Closest::setup: Mask %s contains no atoms.\n",
            distanceMask_.MaskString());
    return 1;
  }

  // Figure out what the the total size of the selected solute atoms plus
  // the number of kept solvent atoms is in order to set up the stripped
  // parm.
  //stripMask_ = soluteMask_;
  //mprintf("DEBUG:\t");
  //stripMask.PrintMaskAtoms("stripMask");
  // Since we have ensured all solvent atoms are of uniform size this is just
  // the desired number of kept waters * number solvent atoms in each mol.
  NsolventAtoms *= closestWaters_;
  mprintf("\tKeeping %i solvent atoms.\n",NsolventAtoms);
  // Add space for kept solvent atom #s at end of mask.
  //stripMask_.AddAtomRange( stripMask_.Nselected(), stripMask_.Nselected() + NsolventAtoms );
  //stripMask.AddAtomRange(currentParm->solventMoleculeStart[0], 
  //                       currentParm->solventMoleculeStop[closestWaters-1]);
  //mprintf("DEBUG:\t");
  //stripMask.PrintMaskAtoms("stripMaskWsolvent");

  // Store old parm
  //oldParm_ = currentParm;
  NsolventMolecules_ = currentParm->Nsolvent();
 
  // Create stripped Parm
  if (newParm_!=NULL) delete newParm_;
  newParm_ = currentParm->modifyStateByMask(stripMask_);
  if (newParm_==NULL) {
    mprinterr("Error: Closest::setup: Could not create new parmtop.\n");
    return 1;
  }
  newParm_->Summary();

  // Allocate space for new frame
  newFrame_.SetupFrame(newParm_->Natom(), newParm_->Mass());

  // If prefix given then output stripped parm
  if (prefix_!=NULL) {
    std::string newfilename(prefix_);
    newfilename += ".";
    newfilename += currentParm->OriginalFilename();
    mprintf("\tWriting out amber topology file %s to %s\n",newParm_->c_str(),
            newfilename.c_str());
    ParmFile pfile;
    pfile.SetDebug( debug );
    if ( pfile.Write(*newParm_, newfilename, ParmFile::AMBERPARM ) ) {
      mprinterr("Error: CLOSEST: Could not write out stripped parm file %s\n",
              newParm_->c_str());
    }
  }

  // Set parm
  currentParm = newParm_;

  return 0;  
}

// Closest::action()
/** Find the minimum distance between atoms in distanceMask and each 
  * solvent Mask.
  */
int Closest::action() {
  int solventMol, maskPosition; //, maxSolventMolecules;
  double Dist, maxD, ucell[9], recip[9];
  AtomMask::const_iterator solute_atom, solvent_atom;

  if (imageType_ != Frame::NOIMAGE) {
    currentFrame->BoxToRecip(ucell, recip);
    // Calculate max possible imaged distance
    maxD = currentFrame->MaxImagedDistance();
  } else {
    // If not imaging, set max distance to an arbitrarily large number
    maxD = DBL_MAX;
  }

  // Loop over all solvent molecules in original frame
  //maxSolventMolecules = oldParm_->Nsolvent();
  // DEBUG
  //mprintf("Closest: Begin parallel loop for %i\n",frameNum);
  // DEBUG
#ifdef _OPENMP
#pragma omp parallel private(solventMol,solute_atom,Dist,solvent_atom)
{
  //mprintf("OPENMP: %i threads\n",omp_get_num_threads());
#pragma omp for
#endif
  for (solventMol=0; solventMol < NsolventMolecules_; solventMol++) {
    //mprintf("[%i] Calculating distance for molecule %i\n",omp_get_thread_num(),solventMol);
    // Set the initial minimum distance for this solvent mol to be the
    // max possible distance.
    SolventMols_[solventMol].D = maxD;
    // DEBUG - show solvent mask
    //fprintf(stdout,"      Solvent %i %i %i\n", MaskList[solventMol]->Selected[0]+1,
    //        MaskList[solventMol]->Selected[1]+1,MaskList[solventMol]->Selected[2]+1);

    // Calculate distance between each atom in distanceMask and atoms in solvent Mask
    solvent_atom = SolventMols_[solventMol].mask.begin();
    for (solute_atom = distanceMask_.begin(); solute_atom != distanceMask_.end(); solute_atom++)
    {
      Dist = currentFrame->DIST2(*solute_atom, *solvent_atom, imageType_, ucell, recip);
      if (Dist < SolventMols_[solventMol].D) 
        SolventMols_[solventMol].D = Dist;
      //fprintf(stdout,"D atom %i %i = %lf image %i\n",*solute_atom,*solvent_atom,minD,imageType);
      // Check the rest of the solvent atoms if specified
      if (!firstAtom_) {
        ++solvent_atom;
        for (; solvent_atom != SolventMols_[solventMol].mask.end(); solvent_atom++) 
        {
          Dist = currentFrame->DIST2(*solute_atom, *solvent_atom, imageType_, ucell, recip);
          if (Dist < SolventMols_[solventMol].D) 
            SolventMols_[solventMol].D = Dist;
        }
      }
    }

    // DEBUG - Print distances
    //mprintf("DEBUG:\tMol %8i minD= %lf\n",solventMol, SolventMols[solventMol].D);
  } // END for loop over solventMol
#ifdef _OPENMP
} // END pragma omp parallel
#endif
  // DEBUG
  //mprintf("Closest: End parallel loop for %i, got %i Distances.\n",frameNum,(int)SolventMols.size());
  // DEBUG

  // Sort distances
  sort( SolventMols_.begin(), SolventMols_.end(), moldist_cmp() );
  // DEBUG
  //mprintf("Closest: Distances sorted for %i\n",frameNum);
  // DEBUG

  // Add first closestWaters solvent atoms to stripMask
  solventMol = 0;
  //maskPosition = soluteMask_.Nselected();
  std::vector<int>::iterator katom = keptWaterAtomNum_.begin();
  for ( std::vector<MolDist>::iterator solvent = SolventMols_.begin();
                                       solvent != SolventMols_.end();
                                       solvent++ ) 
  {
    //mprintf("DEBUG:\tmol %i ",(*solvent).mol);
    //(*solvent).mask.PrintMaskAtoms("Mask");
    maskPosition = *katom;
    ++katom;
    stripMask_.AddMaskAtPosition( (*solvent).mask, maskPosition );

    // Record which water molecules are closest if requested
    if (outFile_!=NULL) {
      int fnum = frameNum + 1;
      framedata_->Add(Nclosest_, &fnum);
      moldata_->Add(Nclosest_, &((*solvent).mol));
      Dist = sqrt( (*solvent).D );
      distdata_->Add(Nclosest_, &Dist);
      solvent_atom = (*solvent).mask.begin();
      int solvent_first_atom = *solvent_atom + 1; 
      atomdata_->Add(Nclosest_, &solvent_first_atom);
      ++Nclosest_;
    }
    // DEBUG - print first closestWaters distances
    //mprintf("DEBUG: Mol %i   D2= %lf   Atom0= %i\n",(*it).mol, (*it).D, (*it).mask->Selected[0]);
    ++solventMol;
    if (solventMol==closestWaters_) break;
  }

  // Modify and set frame
  //mprintf("DEBUG:\t");
  //stripMask.PrintMaskAtoms("action_stripMask");
  newFrame_.SetFrame(*currentFrame, stripMask_);
  currentFrame = &newFrame_;

  return 0;
} 

// Closest::print()
/** Set up the closest output file for writing. Since the datasets used in
  * closest are local and not part of the master data set list, call Sync
  * for them here. Also set so that the X column (which is just an index)
  * is not written.
  */
void Closest::print() {
  if (outFile_==NULL) return;
  // Sync up the dataset list here since it is not part of the master 
  // dataset list.
  outList_.Sync();
  // Set specific datafile options
  outFile_->ProcessArgs("noxcol");
}

