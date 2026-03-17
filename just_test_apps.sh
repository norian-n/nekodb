#!/bin/bash

./clean.sh

# rebuild
echo ============ build test apps only ============

make -f Makefile.tests

echo ============ run test apps ============

mv tests/*.exe tests_exec/

cd tests_exec
# exec all *.exe to log file
for i in *.exe
do 
  LD_LIBRARY_PATH=~/egdb/src ./$i 2>&1 | tee -a all_tests.log 
done
echo
# mv tests_exec/all_tests.log ./

PASSED_COUNT=`cat all_tests.log | grep PASS | wc -l`	
echo 'PASS  total: '$PASSED_COUNT

FAILED_COUNT=`cat all_tests.log | grep FAIL | wc -l`
echo 'FAIL  total: '$FAILED_COUNT
echo
DEV_COUNT=`cat all_tests.log | grep DEVEL | wc -l`	
echo 'DEVEL total: '$DEV_COUNT

ERROR_COUNT=`cat all_tests.log | grep ERROR | wc -l`
echo 'ERROR total: '$ERROR_COUNT' (-2 ERROR(s) in negative tests)'
echo

cd ..

