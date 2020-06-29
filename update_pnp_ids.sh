#!/bin/sh
wget https://uefi.org/uefi-pnp-export -O pnp_ids_temp.tmp
if [ $? -eq 0 ]; then
	sed -r -e s/'<tr class="(.+)"><td>'/'{"'/ -e 's:<td>[[:digit:]]{2}/[[:digit:]]{2}/[[:digit:]]{4}</td>::g' -e s/'(<\/td><td>)'/\",\"/g -e s/'<\/td> <\/tr>'/'\"},'/ pnp_ids_temp.tmp | grep -v '<' > pnp_ids.h
fi
rm pnp_ids_temp.tmp
	
