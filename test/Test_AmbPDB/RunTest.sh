#!/bin/bash

. ../MasterTest.sh

CleanFiles out.pdb out1.pdb out.mol2 fabi.pdb

TESTNAME='AMBPDB tests'
Requires ambpdb

echo ""
echo "  AMBPDB: AmbPDB tests."
if [ -z "$CPPTRAJ_DACDIF" ] ; then
  echo "  AMBPDB: AmbPDB tests." >> $CPPTRAJ_TEST_RESULTS
fi

$VALGRIND $AMBPDB -p ../tz2.parm7 -c ../tz2.rst7 > out.pdb 2> $CPPTRAJ_ERROR
DoTest out.pdb.save out.pdb

$VALGRIND $AMBPDB -p ../tz2.parm7 < ../tz2.rst7 > out1.pdb 2>> $CPPTRAJ_ERROR
DoTest out.pdb.save out1.pdb

UNITNAME='Convert NetCDF to PDB with ambpdb'
CheckFor netcdf
if [ $? -eq 0 ] ; then
  $VALGRIND $AMBPDB -p ../FtuFabI.NAD.TCL.parm7 -c ../FtuFabI.NAD.TCL.nc -bres > fabi.pdb 2>> $CPPTRAJ_ERROR
  DoTest fabi.pdb.save fabi.pdb
fi

UNITNAME='Amber to SYBYL atom type conversion test'
CheckFor amberhome
if [ $? -eq 0 ] ; then
  $VALGRIND $AMBPDB -p ../tz2.parm7 -mol2 -sybyl < ../tz2.rst7 > out.mol2 2>> $CPPTRAJ_ERROR
  DoTest ../Test_Mol2/test2.mol2.save out.mol2
fi

EndTest
exit 0
