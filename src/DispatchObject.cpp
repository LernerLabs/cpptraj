#include "DispatchObject.h"

bool DispatchObject::KeyMatches(const char* keyIn) const {
  for (key_iterator key = keywords_.begin(); key != keywords_.end(); ++key)
    if ( key->compare( keyIn ) == 0 ) return true;
  return false;
}

/** 0 are hidden categories (i.e. should not appear in help). */
const char* DispatchObject::ObjKeyword(Otype typeIn) {
  switch (typeIn) {
    case NONE: return 0;
    case PARM: return "Topology";
    case TRAJ: return "Trajectory";
    case COORDS: return "Coords";
    case ACTION: return "Action";
    case ANALYSIS: return "Analysis";
    case GENERAL: return "General";
    case SYSTEM: return "System";
    case HIDDEN: return 0;
    case DEPRECATED: return 0;
  }
  return 0;
}
