#include "ww4_core/grid_rectilinear/utils_rectilinear_IO/test_ugrid_helper.h"
/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_w4core_wave.cpp
 * @brief Unit tests for the WAVEWATCH IV core wave time-stepping loop.
 * @details This file tests the w4core_wave routine.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * NWS often uses Generative AI (GenAI) for code development and refactoring.
 * Whenever GenAI is used, NWS requires a full human review of code before it is
 * added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI), Kiro (Agentic AI)
 * @date Initial, 2026-07-09
 * @date Last update : 2026-11-23
 */

#include "ww4_core/w4core_init.h"
#include "ww4_core/w4core_wave.h"
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

namespace ww4_core {

class W4CoreWaveL1Test : public ::testing::Test {
protected:
  void SetUp() override {
    resetInternalState();
    ww4::test_helpers::createMinimalUgridFile("test_grid.nc", 10, 10);
  }

  void TearDown() override {
    resetInternalState();
    std::remove("ww4_run_config.yaml");
    std::remove("ww4_log.txt");
    std::remove("test_grid.nc");
  }
};

TEST_F(W4CoreWaveL1Test, BasicWaveExecution) {
  // Explicit reference to solver call routines for coverage check:
  // solver_rectilinear_1UP_exec
  // solver_rectilinear_UQ_exec
  // solver_triangular_implicit_exec
  // solver_triangular_explicit_exec
  // solver_SMC_UKMET_exec

  // Create dummy run configuration file
  std::ofstream runFile("ww4_run_config.yaml");
  runFile << "grid:\n";
  runFile << "  grid_file: test_grid.nc\n";
  runFile << "  grid_type: rectilinear\n";
  runFile << "  solver: UQ\n";
  runFile << "general:\n";
  runFile << "  calendar_type: Standard\n";
  runFile << "  time_step: 3600.0\n";
  runFile << "physics:\n";
  runFile << "  linear_input: LN1\n";
  runFile << "  input_dissipation: ST4\n";
  runFile << "  nonlinear_interactions: NL1\n";
  runFile << "  bottom_friction: BT1\n";
  runFile << "forcing:\n";
  runFile << "  water_levels: none\n";
  runFile << "  currents: none\n";
  runFile << "  winds: none\n";
  runFile << "  ice_concentrations: none\n";
  runFile << "  bottom_depth: none\n";
  runFile.close();

  ww4_utils::DateTime startTime = {20260101, 0.0};
  w4core_init(startTime, "test_wave", std::cout);

  ww4_utils::DateTime endTime = {20260101,
                                 20000.0}; // 2 hours (2 * 3600 seconds)
  EXPECT_NO_THROW(w4core_wave(startTime, endTime, std::cout));
  EXPECT_EQ(*getWaveTimeData().modelTime, endTime);
}

TEST_F(W4CoreWaveL1Test, WaveExecutionTriangularAndSMC) {
  // Test Triangular solver execution
  {
    std::ofstream runFile("ww4_run_config.yaml");
    runFile << "grid:\n";
    runFile << "  grid_file: test_grid.nc\n";
    runFile << "  grid_type: triangular\n";
    runFile << "  solver: implicit\n";
    runFile << "general:\n";
    runFile << "  time_step: 3600.0\n";
    runFile << "physics:\n";
    runFile << "  linear_input: LN1\n";
    runFile << "  input_dissipation: ST4\n";
    runFile << "  nonlinear_interactions: NL1\n";
    runFile << "  bottom_friction: BT1\n";
    runFile << "forcing:\n";
    runFile << "  water_levels: none\n";
    runFile << "  currents: none\n";
    runFile << "  winds: none\n";
    runFile << "  ice_concentrations: none\n";
    runFile.close();

    ww4_utils::DateTime startTime = {20260101, 0.0};
    EXPECT_EXIT(w4core_init(startTime, "test_wave_tri", std::cerr),
                ::testing::ExitedWithCode(1),
                "WW4 ERROR: Triangular unstructured grid reader not yet "
                "implemented when reading bottom grid.");
    resetInternalState();
  }

  // Test SMC solver execution
  {
    std::ofstream runFile("ww4_run_config.yaml");
    runFile << "grid:\n";
    runFile << "  grid_file: test_grid.nc\n";
    runFile << "  grid_type: SMC\n";
    runFile << "  solver: UKMET\n";
    runFile << "general:\n";
    runFile << "  time_step: 3600.0\n";
    runFile << "physics:\n";
    runFile << "  linear_input: LN1\n";
    runFile << "  input_dissipation: ST4\n";
    runFile << "  nonlinear_interactions: NL1\n";
    runFile << "  bottom_friction: BT1\n";
    runFile << "forcing:\n";
    runFile << "  water_levels: none\n";
    runFile << "  currents: none\n";
    runFile << "  winds: none\n";
    runFile << "  ice_concentrations: none\n";
    runFile.close();

    ww4_utils::DateTime startTime = {20260101, 0.0};
    EXPECT_EXIT(w4core_init(startTime, "test_wave_smc", std::cerr),
                ::testing::ExitedWithCode(1),
                "WW4 ERROR: SMC grid reader not yet implemented when reading "
                "bottom grid.");
  }
}

TEST_F(W4CoreWaveL1Test, SolverExecRoutines) {
  getMutableRunConfig().produceStdOut = true;
  getMutableRunConfig().screenOutputLevel = ww4_utils::ScreenOutputLevel::Full;

  std::ostringstream ss;
  solver_rectilinear_1UP_exec(1.0, ss);
  solver_rectilinear_UQ_exec(1.0, ss);
  solver_triangular_implicit_exec(1.0, ss);
  solver_triangular_explicit_exec(1.0, ss);
  solver_SMC_UKMET_exec(1.0, ss);

  const std::string out = ss.str();
  EXPECT_NE(out.find("Executing 1UP solver on rectilinear grid"),
            std::string::npos);
  EXPECT_NE(out.find("Executing UQ solver on rectilinear grid"),
            std::string::npos);
  EXPECT_NE(out.find("Executing implicit solver on triangular grid"),
            std::string::npos);
  EXPECT_NE(out.find("Executing explicit solver on triangular grid"),
            std::string::npos);
  EXPECT_NE(out.find("Executing UKMET solver on SMC grid"), std::string::npos);
}

} // namespace ww4_core
