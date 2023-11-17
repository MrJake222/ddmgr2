#include <iostream>
using std::cout, std::endl;

#include <yaml-cpp/yaml.h>

#include <parse.hpp>
#include <subnet.hpp>

int main() {
    YAML::Node subnet_conf = YAML::LoadFile("examples/conf.yaml");

    parse::yaml_to_subnet(subnet_conf);
}
