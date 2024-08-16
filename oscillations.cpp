#include "oscillations.h"

#include <cmath>
#include <math.h>
#include <string>

oscillations::spring oscillations::spring_mass_system_from_initial_values(
    const std::map<std::string, double> &params) {
  return spring{object{params.at("m")}, params.at("k"), params.at("x")};
}

oscillations::object::object(double m) : m(m){};

void oscillations::object::calculate(const double t) {}

oscillations::spring::spring(object mass, double k, double x)
    : mass(mass), k(k), A(x){};

// TODO: Should be changed to a numerical solution
// Euler's method doesn't work and the below is not a numerical approach.
void oscillations::spring::calculate(const double t) {
  w = sqrt(k / mass.m);
  x = A * cos(w * t);
  v = -w * A * sin(w * t);
  K = 1.0 / 2 * mass.m * pow(v, 2);
  U = 1.0 / 2 * k * pow(x, 2);
  E = K + U;
}
