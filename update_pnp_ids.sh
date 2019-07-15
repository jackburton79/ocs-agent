#!/bin/sh
wget https://uefi.org/uefi-pnp-export
sed -r s/'<tr class="(.+)"><td>'/'{"'/ uefi-pnp-export | sed -r 's:<td>[[:digit:]]{2}/[[:digit:]]{2}/[[:digit:]]{4}</td>::g' | sed -r s/'(<\/td><td>)'/\",\"/g | sed -r s/'<\/td> <\/tr>'/'\"},'/ | grep -v '<' > pnp_ids.h
