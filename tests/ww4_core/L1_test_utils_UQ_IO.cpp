//+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//-- -- -- + | WAVEWATCH IV, open source, code management by NOAA / NWS |
//+ -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
//-- -- -- +
//
// @file L1_test_utils_UQ_IO.cpp
// @brief Unit tests for NetCDF-UGRID grid file reader module utils_UQ_IO.
// @details Tests the readUQgrid subroutine and UgridGrid container structure.
// Internal helper functions: check_nc_err, read_all_global_attributes,
// read_variable, constexpr, get, ncid_.
//
// @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
// Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks
// of the National Weather Service.
// NWS often uses Generative AI (GenAI) for code development and refactoring.
// Whenever GenAI is used, NWS requires a full human review of code before it
// is added to its repositories.
// @author Main Author(s) : Aldgisl (AI Persona), Hendrik L. Tolman
// @author Contributors : Jules (Agentic AI)
// @date Initial : 2026-10-27
// @date Latest Update : 2026-10-30

#include "ww4_core/solver_uq/utils_UQ_IO/utils_UQ_IO.h"

#include <gtest/gtest.h>
#include <netcdf.h>

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace ww4::io {

class UtilsUQIOTest : public ::testing::Test {
protected:
  void TearDown() override {
    std::error_code ec;
    std::filesystem::remove("test_non_existent.nc", ec);
    std::filesystem::remove("test_invalid.txt", ec);
    std::filesystem::remove("test_missing_dims.nc", ec);
    std::filesystem::remove("test_missing_vars.nc", ec);
    std::filesystem::remove("test_valid_ugrid.nc", ec);
  }
};

TEST_F(UtilsUQIOTest, NonExistentFileReturnsStatus1) {
  UgridGrid grid;
  const int status = readUQgrid("test_non_existent.nc", grid);
  EXPECT_EQ(status, 1);
  EXPECT_EQ(grid.ny, 0u);
}

TEST_F(UtilsUQIOTest, InvalidNetCDFFileReturnsStatus1) {
  std::ofstream ofs("test_invalid.txt");
  ofs << "This is not a NetCDF file\n";
  ofs.close();

  UgridGrid grid;
  const int status = readUQgrid("test_invalid.txt", grid);
  EXPECT_EQ(status, 1);
  EXPECT_EQ(grid.nx, 0u);
}

TEST_F(UtilsUQIOTest, MissingDimensionsReturnsStatus2) {
  const std::string filename = "test_missing_dims.nc";
  int ncid = -1;
  ASSERT_EQ(nc_create(filename.c_str(), NC_CLOBBER, &ncid), NC_NOERR);

  int dim_ny = -1;
  ASSERT_EQ(nc_def_dim(ncid, "ny", 2, &dim_ny), NC_NOERR);
  // Deliberately omit 'nx', 'nMesh2_face', 'nMesh2_node',
  // 'nMaxMesh2_face_nodes'
  ASSERT_EQ(nc_close(ncid), NC_NOERR);

  UgridGrid grid;
  const int status = readUQgrid(filename, grid);
  EXPECT_EQ(status, 2);
  EXPECT_EQ(grid.n_faces, 0u);
}

TEST_F(UtilsUQIOTest, MissingRequiredVariablesReturnsStatus3) {
  const std::string filename = "test_missing_vars.nc";
  int ncid = -1;
  ASSERT_EQ(nc_create(filename.c_str(), NC_CLOBBER, &ncid), NC_NOERR);

  int dim_ny = -1, dim_nx = -1, dim_face = -1, dim_node = -1,
      dim_max_nodes = -1;
  ASSERT_EQ(nc_def_dim(ncid, "ny", 2, &dim_ny), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nx", 3, &dim_nx), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_face", 6, &dim_face), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_node", 12, &dim_node), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMaxMesh2_face_nodes", 4, &dim_max_nodes),
            NC_NOERR);

  ASSERT_EQ(nc_close(ncid), NC_NOERR);

  UgridGrid grid;
  const int status = readUQgrid(filename, grid);
  EXPECT_EQ(status, 3);
  EXPECT_EQ(grid.ny, 0u);
}

TEST_F(UtilsUQIOTest, ValidNetCDFUGridParsingSuccess) {
  const std::string filename = "test_valid_ugrid.nc";
  int ncid = -1;
  ASSERT_EQ(nc_create(filename.c_str(), NC_CLOBBER, &ncid), NC_NOERR);

  // Define dimensions
  int dim_ny = -1, dim_nx = -1, dim_face = -1, dim_node = -1,
      dim_max_nodes = -1;
  ASSERT_EQ(nc_def_dim(ncid, "ny", 2, &dim_ny), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nx", 3, &dim_nx), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_face", 6, &dim_face), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_node", 12, &dim_node), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMaxMesh2_face_nodes", 4, &dim_max_nodes),
            NC_NOERR);

  // Write global attributes
  const std::string title = "Test Grid Title";
  const std::string source = "WW4 Test Suite";
  const std::string grid_type = "curvilinear";
  const std::string coord_type = "spherical";
  const std::string global_closure = "none";
  const std::string bathy_source = "test_bathy";
  const std::string bound_source = "test_bound";
  const std::string conventions = "UGRID-1.0";
  const std::string history = "Created by L1_test_utils_UQ_IO";

  ASSERT_EQ(
      nc_put_att_text(ncid, NC_GLOBAL, "title", title.size(), title.c_str()),
      NC_NOERR);
  ASSERT_EQ(
      nc_put_att_text(ncid, NC_GLOBAL, "source", source.size(), source.c_str()),
      NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "grid_type", grid_type.size(),
                            grid_type.c_str()),
            NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "coord_type", coord_type.size(),
                            coord_type.c_str()),
            NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "global_closure",
                            global_closure.size(), global_closure.c_str()),
            NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "bathy_source",
                            bathy_source.size(), bathy_source.c_str()),
            NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "bound_source",
                            bound_source.size(), bound_source.c_str()),
            NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "Conventions", conventions.size(),
                            conventions.c_str()),
            NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "history", history.size(),
                            history.c_str()),
            NC_NOERR);

  // Define 2D variables
  int var_grid_lon = -1, var_grid_lat = -1, var_grid_depth = -1,
      var_grid_mask = -1;
  int var_grid_obsx = -1, var_grid_obsy = -1;
  int dims_2d[2] = {dim_ny, dim_nx};

  ASSERT_EQ(nc_def_var(ncid, "grid_lon", NC_DOUBLE, 2, dims_2d, &var_grid_lon),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "grid_lat", NC_DOUBLE, 2, dims_2d, &var_grid_lat),
            NC_NOERR);
  ASSERT_EQ(
      nc_def_var(ncid, "grid_depth", NC_FLOAT, 2, dims_2d, &var_grid_depth),
      NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "grid_mask", NC_INT, 2, dims_2d, &var_grid_mask),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "grid_ObsX", NC_FLOAT, 2, dims_2d, &var_grid_obsx),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "grid_ObsY", NC_FLOAT, 2, dims_2d, &var_grid_obsy),
            NC_NOERR);

  // Define 1D face variables
  int var_face_lon = -1, var_face_lat = -1, var_depth = -1, var_mask = -1;
  int var_obsx = -1, var_obsy = -1;
  int dims_face[1] = {dim_face};

  ASSERT_EQ(
      nc_def_var(ncid, "face_lon", NC_DOUBLE, 1, dims_face, &var_face_lon),
      NC_NOERR);
  ASSERT_EQ(
      nc_def_var(ncid, "face_lat", NC_DOUBLE, 1, dims_face, &var_face_lat),
      NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "depth", NC_FLOAT, 1, dims_face, &var_depth),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "mask", NC_INT, 1, dims_face, &var_mask),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "ObsX", NC_FLOAT, 1, dims_face, &var_obsx),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "ObsY", NC_FLOAT, 1, dims_face, &var_obsy),
            NC_NOERR);

  // Define 1D node variables
  int var_node_lon = -1, var_node_lat = -1;
  int dims_node[1] = {dim_node};

  ASSERT_EQ(
      nc_def_var(ncid, "node_lon", NC_DOUBLE, 1, dims_node, &var_node_lon),
      NC_NOERR);
  ASSERT_EQ(
      nc_def_var(ncid, "node_lat", NC_DOUBLE, 1, dims_node, &var_node_lat),
      NC_NOERR);

  // Define connectivity variable
  int var_conn = -1;
  int dims_conn[2] = {dim_face, dim_max_nodes};
  ASSERT_EQ(nc_def_var(ncid, "face_node_connectivity", NC_INT, 2, dims_conn,
                       &var_conn),
            NC_NOERR);

  ASSERT_EQ(nc_enddef(ncid), NC_NOERR);

  // Write 2D data
  std::vector<double> grid_lon_data = {10.0, 11.0, 12.0, 10.0, 11.0, 12.0};
  std::vector<double> grid_lat_data = {50.0, 50.0, 50.0, 51.0, 51.0, 51.0};
  std::vector<float> grid_depth_data = {100.0f, 150.0f, 200.0f,
                                        110.0f, 160.0f, 210.0f};
  std::vector<int32_t> grid_mask_data = {1, 1, 1, 1, 1, 0};
  std::vector<float> grid_obsx_data = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
  std::vector<float> grid_obsy_data = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

  ASSERT_EQ(nc_put_var_double(ncid, var_grid_lon, grid_lon_data.data()),
            NC_NOERR);
  ASSERT_EQ(nc_put_var_double(ncid, var_grid_lat, grid_lat_data.data()),
            NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_grid_depth, grid_depth_data.data()),
            NC_NOERR);
  ASSERT_EQ(
      nc_put_var_int(ncid, var_grid_mask,
                     reinterpret_cast<const int *>(grid_mask_data.data())),
      NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_grid_obsx, grid_obsx_data.data()),
            NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_grid_obsy, grid_obsy_data.data()),
            NC_NOERR);

  // Write 1D face data
  std::vector<double> face_lon_data = {10.0, 11.0, 12.0, 10.0, 11.0, 12.0};
  std::vector<double> face_lat_data = {50.0, 50.0, 50.0, 51.0, 51.0, 51.0};
  std::vector<float> depth_data = {100.0f, 150.0f, 200.0f,
                                   110.0f, 160.0f, 210.0f};
  std::vector<int32_t> mask_data = {1, 1, 1, 1, 1, 0};
  std::vector<float> obsx_data = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f};
  std::vector<float> obsy_data = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

  ASSERT_EQ(nc_put_var_double(ncid, var_face_lon, face_lon_data.data()),
            NC_NOERR);
  ASSERT_EQ(nc_put_var_double(ncid, var_face_lat, face_lat_data.data()),
            NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_depth, depth_data.data()), NC_NOERR);
  ASSERT_EQ(nc_put_var_int(ncid, var_mask,
                           reinterpret_cast<const int *>(mask_data.data())),
            NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_obsx, obsx_data.data()), NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_obsy, obsy_data.data()), NC_NOERR);

  // Write 1D node data
  std::vector<double> node_lon_data(12, 10.0);
  std::vector<double> node_lat_data(12, 50.0);
  ASSERT_EQ(nc_put_var_double(ncid, var_node_lon, node_lon_data.data()),
            NC_NOERR);
  ASSERT_EQ(nc_put_var_double(ncid, var_node_lat, node_lat_data.data()),
            NC_NOERR);

  // Write connectivity data
  std::vector<int32_t> conn_data(24, 1);
  ASSERT_EQ(nc_put_var_int(ncid, var_conn,
                           reinterpret_cast<const int *>(conn_data.data())),
            NC_NOERR);

  ASSERT_EQ(nc_close(ncid), NC_NOERR);

  // Read grid
  UgridGrid grid;
  const int status = readUQgrid(filename, grid);
  EXPECT_EQ(status, 0);

  // Verify metadata
  EXPECT_EQ(grid.ny, 2u);
  EXPECT_EQ(grid.nx, 3u);
  EXPECT_EQ(grid.n_faces, 6u);
  EXPECT_EQ(grid.n_nodes, 12u);
  EXPECT_EQ(grid.max_face_nodes, 4u);

  EXPECT_EQ(grid.title, title);
  EXPECT_EQ(grid.source, source);
  EXPECT_EQ(grid.grid_type, grid_type);
  EXPECT_EQ(grid.coord_type, coord_type);
  EXPECT_EQ(grid.global_closure, global_closure);
  EXPECT_EQ(grid.bathy_source, bathy_source);
  EXPECT_EQ(grid.bound_source, bound_source);
  EXPECT_EQ(grid.conventions, conventions);
  EXPECT_EQ(grid.history, history);

  EXPECT_TRUE(grid.has_obsx);
  EXPECT_TRUE(grid.has_obsy);

  // Verify arrays
  EXPECT_EQ(grid.grid_lon.size(), 6u);
  EXPECT_DOUBLE_EQ(grid.grid_lon[0], 10.0);
  EXPECT_FLOAT_EQ(grid.grid_depth[0], 100.0f);
  EXPECT_EQ(grid.grid_mask[5], 0);

  EXPECT_EQ(grid.depth.size(), 6u);
  EXPECT_FLOAT_EQ(grid.depth[1], 150.0f);

  EXPECT_EQ(grid.face_node_connectivity.size(), 24u);

  // Verify clear routine
  grid.clear();
  EXPECT_EQ(grid.ny, 0u);
  EXPECT_EQ(grid.nx, 0u);
  EXPECT_EQ(grid.n_faces, 0u);
  EXPECT_TRUE(grid.title.empty());
  EXPECT_FALSE(grid.has_obsx);
  EXPECT_TRUE(grid.grid_lon.empty());
}

} // namespace ww4::io
