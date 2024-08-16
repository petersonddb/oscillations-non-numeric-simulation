#include <gtest/gtest.h>
#include <map>
#include <string>
#include <vector>

#include "oscillations.h"

TEST(SimpleHarmonicMotion, SpringMassSystem) {
  std::map<std::string, double> params{{"k", 10.0}, {"m", 80.0}, {"x", 2.0}};

  oscillations::spring spring_mass_system(
      oscillations::spring_mass_system_from_initial_values(params));

  int t(2);
  spring_mass_system.calculate(t);

  EXPECT_NEAR(0.35, spring_mass_system.w, 0.01);
  EXPECT_NEAR(1.52, spring_mass_system.x, 0.01);
  EXPECT_NEAR(20, spring_mass_system.E, 0.01);

  t = 9;
  spring_mass_system.calculate(t);
  
  EXPECT_NEAR(0.35, spring_mass_system.w, 0.01);
  EXPECT_NEAR(-1.99837, spring_mass_system.x, 0.001);
  EXPECT_NEAR(20, spring_mass_system.E, 0.01);
}
