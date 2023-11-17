#include "parse.hpp"

#include <host.hpp>

#include <iostream>
using std::cout;
using std::endl;

namespace parse {

class parse_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

bool is_number(const std::string& str) {
    return std::all_of(
            str.begin(),
            str.end(),
            [](unsigned char c){ return std::isdigit(c); });
}

bool is_hex_number(const std::string& str) {
    return std::all_of(
            str.begin(),
            str.end(),
            [](unsigned char c){ return std::isxdigit(c); });
}

std::vector<std::string> to_groups(const std::string& in, char delim,
                                   const std::string& error_tag,
                                   int group_count_min, int group_count_max,
                                   verify_group_fn verify) {
    std::stringstream inss(in);
    std::string group;
    int group_idx = 0;
    std::vector<std::string> groups;
    while (getline(inss, group, delim)) {
//        if (group.length() < group_size_min || group.length() > group_size_max)
//            goto invalid;
//        if (hex && !is_hex_number(group))
//            goto invalid;
//        if (number && !is_number(group))
//            goto invalid;

        if (!verify(group_idx, group))
            goto invalid;

        groups.push_back(group);
        group_idx++;
    }

    if (groups.size() < group_count_min || groups.size() > group_count_max)
        goto invalid;

    return groups;

    invalid:
    throw parse_error("invalid " + error_tag + " group parsing: " + in);
}

std::string checked_as(const YAML::Node& conf, const std::string& name, const std::string& error_suffix) {
    if (!conf[name])
        throw parse_error("required field "+name+" missing " + error_suffix);

    return conf[name].as<std::string>();
}

Subnet yaml_to_subnet(YAML::Node conf) {

    std::vector<Host> hosts;

    auto v4_prefix   = checked_as(conf, "v4_prefix", "");
    auto v4_external = checked_as(conf, "v4_external", "");
    auto v6_prefix   = checked_as(conf, "v6_prefix", "");
    auto domain      = checked_as(conf, "domain", "");

    for (auto hostmap: conf["hosts"]) {
        // hostmap is a key-value map, key being arbitrary value not to be used
        // value is what counts, contains all important information about the host
        auto hostinfo = hostmap.second;

        // keys of host to be parsed:
        // required: name, mac, ipv4
        // optional: ipv6, ports, aname, cname, mx

        auto name = checked_as(hostinfo, "name", "on host");
        auto mac  = checked_as(hostinfo, "mac", "on host " + name);
        auto ipv4 = checked_as(hostinfo, "ipv4", "on host " + name);

        hosts.emplace_back(
                name,
                parse_mac(name, hostinfo["mac"].as<std::string>()),
                parse_ipv4(name, v4_prefix + hostinfo["ipv4"].as<std::string>()),

                hostinfo["ipv6"]
                    ? std::optional<dt::IPv6>{parse_ipv6(name, v6_prefix + hostinfo["ipv6"].as<std::string>())}
                    : std::nullopt,

                hostinfo["ports"]
                    ? parse_ports(name, hostinfo["ports"].as<std::vector<std::string>>())
                    : std::vector<dt::PortMap>{},

                hostinfo["aname"]
                    ? hostinfo["aname"].as<std::vector<std::string>>()
                    : std::vector<std::string>{},

                hostinfo["cname"]
                    ? hostinfo["cname"].as<std::vector<std::string>>()
                    : std::vector<std::string>{},

                hostinfo["mx"]
                    ? std::optional<int>{hostinfo["mx"].as<int>()}
                    : std::nullopt

                );

        print_host(hosts.back());
    }

    return { hosts };
}

dt::MAC parse_mac(const std::string& host, const std::string& in) {

    verify_group_fn verify = [](int idx, const std::string& group) {
        if (group.length() > 2)
            return false;

        if (!is_hex_number(group))
            return false;

        return true;
    };

    std::vector<std::string> groups =
            to_groups(in, ':',
                            host + " mac",
                            6, 6,
                            verify);

    return {in, groups};
}

dt::IPv4 parse_ipv4(const std::string& host, const std::string& in) {

    verify_group_fn verify = [](int idx, const std::string& group) {
        if (group.length() > 3)
            return false;

        if (!is_number(group))
            return false;

        int val = stoi(group);
        if (val > 255 || val < 0)
            return false;

        return true;
    };

    std::vector<std::string> groups =
            to_groups(in, '.',
                            host + " ipv4",
                            4, 4,
                            verify);

    return {in, groups};
}

dt::IPv6 parse_ipv6(const std::string& host, const std::string& in) {

    verify_group_fn verify = [](int idx, const std::string& group) {
        if (group.length() > 4)
            return false;

        if (!is_hex_number(group))
            return false;

        return true;
    };

    std::vector<std::string> groups =
            to_groups(in, ':',
                            host + " ipv6",
                            1, 8,
                            verify);

    // find the empty group
    int empty_index = -1;
    for (int i=0; i<groups.size(); i++) {
        if (groups[i].length() == 0) {
            if (empty_index == -1)
                // first empty group
                empty_index = i;
            else
                // second empty index (invalid)
                goto invalid;
        }
    }

    while (groups.size() < 8)
        groups.insert(groups.begin() + empty_index, "0000");

    return {in, groups};

invalid:
    throw parse_error("invalid " + host + " ipv6 parsing: " + in);
}

std::vector<dt::PortMap> parse_ports(const std::string& host, const std::vector<std::string>& ports) {

    std::vector<dt::PortMap> maps;

    verify_group_fn verify = [](int idx, const std::string& group) {
        if (idx == 1 || idx == 2) {
            // ports
            if (!is_number(group))
                return false;

            int val = stoi(group);
            if (val < 0 || val > 65535)
                return false;
        }

        return true;
    };

    for (const auto& port : ports) {
        std::vector<std::string> g =
                to_groups(port, ':',
                                host + " ports",
                                3, 3,
                                verify);

        maps.emplace_back(g[0], stoi(g[1]), stoi(g[2]));
    }

    return maps;
}

void print_host(const Host& host) {
    cout << "host name=" << host.name << endl;
    cout << "  mac=" << host.mac.orig << endl;
    cout << "  ipv4=" << host.ipv4.orig << endl;
    cout << "  ipv6=" << (host.ipv6 ? host.ipv6->orig : "[no v6]") << endl;

    cout << "  ports=";
    for (const auto& m : host.ports)
        cout << m.from << "->" << m.to << "/" << m.proto << " ";
    cout << endl;

    cout << "  aname=";
    for (const auto& m : host.aname)
        cout << m << " ";
    cout << endl;

    cout << "  cname=";
    for (const auto& m : host.cname)
        cout << m << " ";
    cout << endl;

    if (host.mx)
        cout << "  mx=" << *host.mx << endl;

    cout << endl;
}

} // namespace
