#!/bin/bash
set -e

####
#
# This script is designed to run tests for the NWChemExBase repo.  It's usage
# is:
#
#   ./RunNWChemExBaseTest.sh <test_name>
#
#   In the build directory, this script will perform the following steps:
#   1. make a directory called <test_name> and change to it
#   2. run BasicSetup.sh <test_name>
#   3. copy header and source files to <test_name>/<test_name>
#   4. Make a single test in <test_name>_Test that calls run_test() on your lib
#

test_name=${1}
cmake_command=${2}

header_file=${test_name}.hpp
source_file=${test_name}.cpp

# Establish the testing directory by appending a number (will try up to 10)
for ((i=1;i<10;i+=1));do
    test_dir="${test_name}_${i}"
    if [ ! -d ${test_dir} ];then
       echo "Testing occurs in ${test_dir}"
       mkdir ${test_dir}
       break
    fi
    if [ ${i} -eq 9 ];then
      echo "Too many runs, clean your build directory!!!!!"
      exit 1
    fi
done

#Change to and setup the build directory
cd ${test_dir}
../../../bin/BasicSetup.sh ${test_name}
ln -s ../../../../NWChemExBase #Pretend NWChemExBase is a sub-folder

#Make the header file
echo "#pragma once">${test_name}/${header_file}
echo "struct ${test_name} { void run_test();};">>${test_name}/${header_file}

cp ../${test_name}_files/${source_file} ${test_name} #Copy source

#Add the sources and headers to the CMakeLists.txt
srcs_line="set(${test_name}_SRCS"
srcs_replace="${srcs_line} ${source_file}"
sed -i "s/${srcs_line}/${srcs_replace}/g" ${test_name}/CMakeLists.txt
incs_line="set(${test_name}_INCLUDES"
incs_replace="${incs_line} ${header_file}"
sed -i "s/${incs_line}/${incs_replace}/g" ${test_name}/CMakeLists.txt



#Add the tests
test_cmake=${test_name}_Test/CMakeLists.txt
test_src=${test_name}_Test/Test${source_file}
echo "add_cxx_unit_test(Test${test_name} ${test_name})" >> ${test_cmake}
echo "#include<${test_name}/${header_file}>" > ${test_src}
echo "int main(){ ${test_name} temp; temp.run_test(); return 0;}">> ${test_src}

#Set the cache file
cache_file=../${test_name}_files/CMakeVars.txt

#Run CMake
cmake -Bbuild -C${cache_file} -H.

#Build the files
cd build
make

#Run the test
ctest
