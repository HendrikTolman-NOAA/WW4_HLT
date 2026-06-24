/**
 * @file test_wave_model.cpp
 * @brief Integration tests for the WaveModel class with integrated physics.
 */

#include "ww4_core/wave_model.hpp"
#include "ww4_core/scheme_factory.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace ww4_core {

TEST(WaveModelTest, SimulationStep) {
  WaveModel model;

  // Assemble the model: PR3 solver with ST4 source term
  auto solver = SchemeFactory::createSolver("PR3");
  auto source = SchemeFactory::createSourceTerm("ST4");
  solver->addSourceTerm(std::move(source));

  model.initialize(std::move(solver));

  std::vector<double> initialData = {1.0, 2.0, 3.0};
  model.setData(initialData);

  model.step();

  auto result = model.getData();
  ASSERT_EQ(result.size(), initialData.size());

  // PR3 multiplies by 1.01, ST4 adds 0.5
  for (size_t i = 0; i < initialData.size(); ++i) {
    double expected = initialData[i] * 1.01 + 0.5;
    EXPECT_NEAR(result[i], expected, 1e-9);
  }
}

} // namespace ww4_core
