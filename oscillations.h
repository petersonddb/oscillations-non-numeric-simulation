#ifndef OSCILLATIONS_H
#define OSCILLATIONS_H

#include <map>
#include <string>

namespace oscillations {
class physical_component {
public:
  virtual void calculate(const double t) = 0;
};

class object : physical_component {
public:
  double m; // mass

  object(double m);
  void calculate(const double t);
};

class spring : physical_component {
public:
  object mass;
  double x, v, k, w, E, K, U, A;

  spring(object mass, double k, double x);
  void calculate(const double t);
};

spring
spring_mass_system_from_initial_values(const std::map<std::string, double> &params);
} // namespace oscillations

#endif // !OSCILLATIONS_H
