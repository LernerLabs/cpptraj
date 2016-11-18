#include "Topic.h"
#include "TrajectoryFile.h"
#include "CpptrajStdio.h"

void Topic_Trajout::PrintTopic(ArgList& argIn) const {
  //TrajectoryFile::WriteOptions();
  std::string key = argIn.GetStringNext();
  if (key.empty()) {
    TrajectoryFile::ListFormats();
    mprintf("  Enter 'help trajout_format <keyword> for in-depth format information.\n");
  } else {
    if (TrajectoryFile::WriteOptions( key ))
      mprintf("Unrecognized format '%s'.\n", key.c_str());
  }
}
