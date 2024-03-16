#include "flags.hpp"

DEFINE_bool(
        verbose, false,
        "show debugging messages on stdout");

DEFINE_string(dhcp,    "", "output dhcp file"   );
DEFINE_string(dhcpv6,  "", "output dhcpv6 file" );
DEFINE_string(dnsext,  "", "output dnsext file" );
DEFINE_string(dnsint,  "", "output dnsint file" );
DEFINE_string(dnsrev,  "", "output dnsrev file");
DEFINE_string(dnsrev6, "", "output dnsrev6 file");
DEFINE_string(nft,     "", "output nft file"    );

DEFINE_int32(dnsrev_mask,  0, "dnsrev  entry subnet mask to generate");
DEFINE_int32(dnsrev6_mask, 0, "dnsrev6 entry subnet mask to generate");

DEFINE_string(nft_chain_filter_forward, "FILTER FORWARD", "nftables forwarding chain name");
DEFINE_string(nft_chain_nat_prerouting, "NAT PREROUTING", "nftables nat prerouting chain name");
DEFINE_string(nft_net_internal, "$INTERNAL", "nftables internal networks macro");