#include <iostream>
using std::cout, std::endl;
using std::cerr;

#include <fstream>
#include <flags.hpp>

#include <yaml-cpp/yaml.h>
#include <parse.hpp>
#include <subnet.hpp>

#include <output.hpp>

void check_write(const std::string& file, const std::string& name, output::fptr outfn, const Subnet& subnet) {
    if (file.length() > 0) {
        cout << "gen " << name << " -> " << file << endl;
        std::ofstream of(file, std::ios::app);
        outfn(of, subnet);
        of.close();
    }
}

int main(int argc, char** argv) {
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    try {
        if (argc != 2) {
            cout << "subnet file is required" << endl;
            return 1;
        }

        const YAML::Node subnet_conf = YAML::LoadFile(argv[1]);

        cout << "parsing input" << endl;
        const Subnet subnet = parse::yaml_to_subnet(subnet_conf);
        cout << "done parsing input" << endl;

        // check each output flag and append to the files provided
        check_write(FLAGS_dhcp,    "dhcp   ", output::dhcp,    subnet);
        check_write(FLAGS_dhcpv6,  "dhcpv6 ", output::dhcpv6,  subnet);
        check_write(FLAGS_dnsext,  "dnsext ", output::dnsext,  subnet);
        check_write(FLAGS_dnsint,  "dnsint ", output::dnsint,  subnet);
        check_write(FLAGS_dnsrev6, "dnsrev6", output::dnsrev6, subnet);
        check_write(FLAGS_nft,     "nft    ", output::nft,     subnet);
    }
    catch (const YAML::Exception& e) {
        cerr << "yaml parser error: " << e.what() << endl;
        return 1;
    }
    catch (const parse::parse_error& e) {
        cerr << "subnet parser error: " << e.what() << endl;
        return 1;
    }
}
