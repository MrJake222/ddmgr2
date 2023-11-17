#pragma once

#include <gflags/gflags.h>

// verbose printing
DECLARE_bool(verbose);

// output files
DECLARE_string(dhcp);
DECLARE_string(dhcpv6);
DECLARE_string(dnsext);
DECLARE_string(dnsint);
DECLARE_string(dnsrev6);
DECLARE_string(nft);

// dns rev v6
DECLARE_int32(dnsrev6_mask);

// nft chains
DECLARE_string(nft_chain_filter_forward);
DECLARE_string(nft_chain_nat_prerouting);
// nft internal networks
DECLARE_string(nft_net_internal);