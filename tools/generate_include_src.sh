#! /bin/sh

#
echo "Only run after making major changes to reset list of include and source files"
#
echo "Include Files to check"
echo
ls ../include/ | grep -v rtl_433_devices.h | grep -v "^log.h"
echo
rm include_copy_list include_copy_and_edit_list src_copy_and_edit_list src_copy_list
for i in `ls ../include/ | grep -v rtl_433_devices.h | grep -v "^log.h"`
do
    echo
    # echo "Checking " $i
    diff ../include/$i ../rtl_433/include/$i >> /dev/null
    retVal=$?
    if [ $retVal -ne 0 ]; then
        echo "File "$i "was changed"
        echo $i >> include_copy_and_edit_list
    else
        echo "File "$i "was not changed"
        echo $i >> include_copy_list
    fi
done

echo
echo "-----------------------------------------------------------------------------------------"
echo
echo "Source Files to check"
echo
ls ../src/rtl_433 | grep -v devices
echo
for i in `ls ../src/rtl_433 | grep -v devices`
do
    echo
    echo "Checking " $i
    diff ../src/rtl_433/$i ../rtl_433/src/$i >> /dev/null
    retVal=$?
    if [ $retVal -ne 0 ]; then
        echo "File "$i "was changed"
        echo $i >> src_copy_and_edit_list
    else
        echo "File "$i "was not changed"
        echo $i >> src_copy_list
    fi
done