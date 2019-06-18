#!/bin/sh
wget https://uefi.org/uefi-pnp-export
cat uefi-pnp-export | sed -r s/'<tr class="(.+)"><td>'/'{"'/  | sed -r s/'(<\/td><td>)'/\",\"/g | sed -r s/'<\/td> <\/tr>'/'\"},'/ | grep -v '<' > pnp_ids.h

