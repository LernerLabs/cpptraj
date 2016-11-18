#include "IOtokenList.h"

IOtokenList::~IOtokenList() { Clear(); }

void IOtokenList::Clear() {
  for (IOarray::iterator it = ioList_.begin(); it != ioList_.end(); ++it)
    it->Clear();
  ioList_.clear();
} 
