#!/bin/sh
wget https://uefi.org/uefi-pnp-export -O pnp_ids_temp.tmp
if [ $? -eq 0 ]; then
	sed -E -e 's:<thead>::g' -e 's:<tr.*="(.+)"><td>:{ ":g' -e 's:<td>[[:digit:]]{2}/[[:digit:]]{2}/[[:digit:]]{4}</td>::g' -e 's:(<\/td><td>):\", \":g' -e 's:<\/td>.<\/tr>:\" },:g' -e 's/"(.*?)", "(.*?)"/\"\2\", \"\1\"/' -e 's/&amp;/\&/' -e "s/&#039;/'/" pnp_ids_temp.tmp | grep -v '<' | sort > pnp_ids.h
fi
rm pnp_ids_temp.tmp

