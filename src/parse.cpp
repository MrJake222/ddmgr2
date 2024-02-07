#include "parse.hpp"

#include <host.hpp>

#include <iostream>
using std::cout;
using std::endl;

#include <algorithm>

#include <flags.hpp>

#define MAX(a, b) ((a)>(b) ? (a) : (b))

namespace parse {

namespace {
// private namespace
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

void zfill(std::string& str, size_t to) {
    std::string zeroes;
    for (int i=0; i<to-str.length(); i++)
        zeroes += '0';

    str = zeroes + str;
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

std::string vs2s(const std::vector<std::string>& v) {
    std::string out;
    for (const std::string& e : v) {
        out += e;
        out += " ";
    }
    out.pop_back(); // remove trailing space
    return out;
}

} // end private namespace

Subnet yaml_to_subnet(const YAML::Node& conf) {

    std::vector<Host> hosts;

    std::string v4_prefix   = checked_as(conf, "v4_prefix", "");
    std::string v4_external = checked_as(conf, "v4_external", "");
    std::string v6_prefix   = checked_as(conf, "v6_prefix", "");
    std::string domain      = checked_as(conf, "domain", "");

    int host_name_max_len = 0;

    for (auto hostmap: conf["hosts"]) {
        // hostmap is a key-value map, key being arbitrary value not to be used
        // value is what counts, contains all important information about the host
        auto hostinfo = hostmap.second;

        // keys of host to be parsed:
        // required: name, ipv4
        // optional: mac, ipv6, ports, aname, cname, mx

        std::string name = checked_as(hostinfo, "name", "on host");
        std::string ipv4 = checked_as(hostinfo, "ipv4", "on host " + name);

        hosts.emplace_back(
                name,
                parse_ipv4(name, v4_prefix + hostinfo["ipv4"].as<std::string>()),

                hostinfo["mac"]
                    ? std::optional<dt::MAC>{parse_mac(name, hostinfo["mac"].as<std::string>())}
                    : std::nullopt,

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

                hostinfo["ns"]
                    ? std::optional<dt::NS>{parse_ns(name, hostinfo["ns"]["for"].as<std::string>())}
                    : std::nullopt,

                hostinfo["mx"]
                    ? std::optional<dt::MX>{parse_mx(name,
                                                   hostinfo["mx"]["for"].as<std::string>(),
                                                         hostinfo["mx"]["pri"].as<int>())}
                    : std::nullopt
                );

        const Host& host = hosts.back();

        // calculate max lenght across all aliases of the host
        host_name_max_len = MAX(host_name_max_len, host.name.length());
        for (const std::string& n : host.aname)
            host_name_max_len = MAX(host_name_max_len, n.length());
        for (const std::string& n : host.cname)
            host_name_max_len = MAX(host_name_max_len, n.length());

        if (FLAGS_verbose)
            print_host(host);
    }

    return { v4_prefix,
             v4_external,
             v6_prefix,
             domain,
             hosts,
             host_name_max_len };
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

    // zero-fill groups for ipv6 reverse zones to work
    for (std::string& group : groups)
        zfill(group, 4);

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

    for (const std::string& port : ports) {
        std::vector<std::string> g =
                to_groups(port, ':',
                                host + " ports",
                                2, 3,
                                verify);

        if (g.size() == 2)
            // only proto:port given
            // open v6 port only
            maps.emplace_back(g[0], std::nullopt, stoi(g[1]));

        else
            // proto:external:internal given
            // open port v4/v6, do nat from external to internal
            maps.emplace_back(g[0], std::optional<int>{stoi(g[1])}, stoi(g[2]));
    }

    return maps;
}

dt::NS parse_ns(const std::string &host, const std::string &for_domain) {
    return { for_domain };
}

dt::MX parse_mx(const std::string &host, const std::string &for_domain, int pri) {
    return { for_domain, pri };
}

void print_host(const Host& host) {
    cout << "host name=" << host.name << endl;
    if (host.mac)
        cout << "  mac=" << host.mac->orig << " -> " << vs2s(host.mac->groups) << endl;
    else
        cout << "  [no mac]" << endl;

    cout << "  ipv4=" << host.ipv4.orig << " -> " << vs2s(host.ipv4.groups) << endl;
    if (host.ipv6)
        cout << "  ipv6=" << host.ipv6->orig << " -> " << vs2s(host.ipv6->groups) << endl;
    else
        cout << "  [no ipv6]" << endl;

    cout << "  ports=";
    for (const dt::PortMap& m: host.ports)
        cout << m.pretty() << " ";
    cout << endl;

    cout << "  aname=";
    for (const std::string& m : host.aname)
        cout << m << " ";
    cout << endl;

    cout << "  cname=";
    for (const std::string& m : host.cname)
        cout << m << " ";
    cout << endl;

    if (host.ns)
        cout << "  ns for " << host.ns->for_domain << endl;

    if (host.mx)
        cout << "  mx for " << host.mx->for_domain << " pri " << host.mx->pri << endl;

    cout << endl;
}

} // namespace
