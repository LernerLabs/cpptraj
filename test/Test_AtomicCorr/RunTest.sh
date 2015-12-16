#!/bin/bash

. ../MasterTest.sh

CleanFiles corr.in acorr.gnu

INPUT="-i corr.in"
cat > corr.in <<EOF
parm ../tz2.parm7
trajin ../tz2.nc
rms first
atomiccorr out acorr.gnu
EOF
RunCpptraj "Atomic Correlation test."
DoTest acorr.gnu.save acorr.gnu
CheckTest
EndTest

exit 0

