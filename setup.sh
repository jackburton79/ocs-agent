THINSTATION_PATH=`cat ../../THINSTATION_PATH`

. ../SET_ENV

make clean
make 
strip -p ocsinventory-agent
