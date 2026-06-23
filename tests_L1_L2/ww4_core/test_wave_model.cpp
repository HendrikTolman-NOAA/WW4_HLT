/**
 * @file test_wave_model.cpp
 * @brief Integration tests for the WaveModel class.
 */

#include "ww4_core/wave_model.hpp"
#include <gtest/gtest.h>
#include <vector>

namespace ww4_core {

TEST(WaveModelTest, SimulationStep) {
  WaveModel model;
  model.initialize("PR3", "ST4");

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
