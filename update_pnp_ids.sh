#!/bin/sh
wget https://uefi.org/uefi-pnp-export
sed -r -e s/'<tr class="(.+)"><td>'/'{"'/ -e 's:<td>[[:digit:]]{2}/[[:digit:]]{2}/[[:digit:]]{4}</td>::g' -e s/'(<\/td><td>)'/\",\"/g -e s/'<\/td> <\/tr>'/'\"},'/ uefi-pnp-export | grep -v '<' > pnp_ids.h
