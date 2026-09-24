/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L2_test_wave_model.cpp
 * @brief Integration tests for the WaveModel class with integrated physics.
 * @details Verifies the integration of solvers and source term physics in the
 *          orchestration loop of the main WaveModel.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 *
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 *
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-06-24
 * @date Last update : 2026-09-24
 */

#include "ww4_core/scheme_factory.h"
#include "ww4_core/wave_model.h"
#include <gtest/gtest.h>
#include <vector>

namespace ww4_core {

TEST(WaveModelTest, SimulationStep) {
  WaveModel model;

  // Assemble the model: UQ solver with ComputeAllSources source term
  auto solver = SchemeFactory::createSolver("UQ");
  auto source = SchemeFactory::createSourceTerm("compute_all_sources");
  solver->addSourceTerm(std::move(source));

  model.initialize(std::move(solver));

  std::vector<double> initialData = {1.0, 2.0, 3.0};
  model.setData(initialData);

  model.step();

  auto result = model.getData();
  ASSERT_EQ(result.size(), initialData.size());

  // UQ multiplies by 1.01, ComputeAllSources adds 0.1
  for (size_t i = 0; i < initialData.size(); ++i) {
    double expected = initialData[i] * 1.01 + 0.1;
    EXPECT_NEAR(result[i], expected, 1e-9);
  }
}

} // namespace ww4_core
