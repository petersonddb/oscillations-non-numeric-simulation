#include <fstream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "oscillations.h"

#define FILE_NAME_DATA "oscillation.dat"

using json = nlohmann::json;

void load_params(std::map<std::string, double> &params,
                 const std::string file_name);
void simulate(std::vector<std::vector<double>> &data,
              const std::map<std::string, double> &params);
void store_data(const std::vector<std::vector<double>> &data,
                const std::string file_name);

int main(int argc, char **argv) {
  std::vector<std::vector<double>> data;
  std::map<std::string, double> params;

  load_params(params, argv[1]);
  simulate(data, params);
  store_data(data, FILE_NAME_DATA);
}

void load_params(std::map<std::string, double> &params,
                 const std::string file_name) {
  std::ifstream file(file_name);
  json read_json{json::parse(file)};
  read_json.get_to(params);
}

void simulate(std::vector<std::vector<double>> &data,
              const std::map<std::string, double> &params) {
  oscillations::spring spring_mass_system(
      oscillations::spring_mass_system_from_initial_values(params));

  int n(params.at("t"));
  for (int t = 0; t < n; t++) {
    spring_mass_system.calculate(t);

    data.push_back({spring_mass_system.x, spring_mass_system.E});
  }
}

void store_data(const std::vector<std::vector<double>> &data,
                const std::string file_name) {
  std::ofstream file(file_name);

  for (int t = 0; t < data.size(); t++) {
    file << t << " " << data[t][0] << " " << data[t][1] << std::endl;
  }
}
