#!/bin/bash 

EXCLUDE_LIST="test"

find . -name "*.cpp" | egrep -v $EXCLUDE_LIST | xargs -n 1 ./style_check.sh
find . -name "*.h"   | egrep -v $EXCLUDE_LIST | xargs -n 1 ./style_check.sh
