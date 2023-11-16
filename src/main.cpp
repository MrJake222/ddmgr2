#include <iostream>
using std::cout, std::endl;

#include <yaml-cpp/yaml.h>

int main() {
    YAML::Node config = YAML::LoadFile("examples/conf.yaml");

    cout << config["hosts"]["bastion"];
}
