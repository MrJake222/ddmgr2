#!/bin/bash

# required duplication to get DIR_CONF
DIR_PREFIX="/usr/local"
PROJ_NAME="ddmgr2"
DIR_BIN="${DIR_PREFIX}/bin"
DIR_CONF="${DIR_PREFIX}/etc/${PROJ_NAME}"

. ${DIR_CONF}/main.conf

echo "preparing base files"

# dns + reverse dns
DNS_SERIAL=$(date +"%y%m%d%H%M")
function gs {
    # replace "SERIAL" with correct timestamp
    sed "s/SERIAL/${DNS_SERIAL}/g" $1 > $2
}
gs ${BASE_DNS} ${OUT_DNS}.ext
gs ${BASE_DNS} ${OUT_DNS}.int
gs ${BASE_DNS_REV} ${OUT_DNS_REV}
gs ${BASE_DNS_REV6} ${OUT_DNS_REV6}

# dhcp
cp ${BASE_DHCP} ${OUT_DHCP}
cp ${BASE_DHCPv6} ${OUT_DHCPv6}

# nftables
: > ${OUT_NFTABLES}

for yaml in ${DIR_CONF_SUBNETS}/*; do
	echo "parsing $yaml"
    
    ${FILE_EXEC} ${yaml}            \
        --dhcp ${OUT_DHCP}          \
        --dhcpv6 ${OUT_DHCPv6}      \
        --dnsint ${OUT_DNS}.int     \
        --dnsext ${OUT_DNS}.ext     \
        --dnsrev ${OUT_DNS_REV}     \
        --dnsrev6 ${OUT_DNS_REV6}   \
        --nft ${OUT_NFTABLES}       \
        \
        --dnsrev_mask ${OUT_dnsrev_mask}   \
        --dnsrev6_mask ${OUT_dnsrev6_mask} \
        --nft_chain_filter_forward "${OUT_nft_chain_filter_forward}" \
        --nft_chain_nat_prerouting "${OUT_nft_chain_nat_prerouting}" \
        --nft_net_internal ${OUT_nft_net_internal}
done

