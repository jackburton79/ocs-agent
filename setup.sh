#!/bin/sh

THINSTATION_PATH=`cat ../../THINSTATION_PATH`

. ../SET_ENV

export LDFLAGS="-L/usr/local/ssl/lib"

make clean
make OcsInventory-ng-agent
strip -p ocsinventory-agent
