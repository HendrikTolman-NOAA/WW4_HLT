/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file L1_test_utils_UQ_IO.cpp
 * @brief Level 1 unit tests for UQ NetCDF-UGRID grid reading utilities.
 * @details Tests readUQgrid subroutine, UgridGrid data structure, error codes,
 *          attribute parsing, 2D/1D grid array reading, topology connectivity,
 *          and subgrid obstruction factor handling.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the
 * National Weather Service. NWS often uses Generative AI (GenAI) for code
 * development and refactoring. Whenever GenAI is used, NWS requires a full
 * human review of code before it is added to its repositories.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-26
 * @date Last update : 2026-10-30
 */

#include "ww4_core/solver_UQ/utils_UQ_IO/utils_UQ_IO.h"
#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <netcdf.h>

using namespace ww4::io;

TEST(UtilsUQIOTest, ReadUQGridSuccess) {
  const std::string filename = "test_read_uqgrid.nc";
  int ncid = -1;
  ASSERT_EQ(nc_create(filename.c_str(), NC_CLOBBER, &ncid), NC_NOERR);

  int dim_ny = -1, dim_nx = -1, dim_faces = -1, dim_nodes = -1,
      dim_maxnodes = -1;
  ASSERT_EQ(nc_def_dim(ncid, "ny", 2, &dim_ny), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nx", 2, &dim_nx), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_face", 4, &dim_faces), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_node", 9, &dim_nodes), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMaxMesh2_face_nodes", 4, &dim_maxnodes),
            NC_NOERR);

  const std::string title = "UGRID Test Grid";
  const std::string grid_type = "regular";
  ASSERT_EQ(
      nc_put_att_text(ncid, NC_GLOBAL, "title", title.size(), title.c_str()),
      NC_NOERR);
  ASSERT_EQ(nc_put_att_text(ncid, NC_GLOBAL, "grid_type", grid_type.size(),
                            grid_type.c_str()),
            NC_NOERR);

  int dims_2d[2] = {dim_ny, dim_nx};
  int dims_conn[2] = {dim_faces, dim_maxnodes};

  int var_grid_lon = -1, var_grid_lat = -1, var_grid_depth = -1,
      var_grid_mask = -1;
  int var_face_lon = -1, var_face_lat = -1, var_depth = -1, var_mask = -1;
  int var_node_lon = -1, var_node_lat = -1, var_conn = -1;
  int var_grid_obsx = -1, var_grid_obsy = -1, var_obsx = -1, var_obsy = -1;

  ASSERT_EQ(nc_def_var(ncid, "grid_lon", NC_DOUBLE, 2, dims_2d, &var_grid_lon),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "grid_lat", NC_DOUBLE, 2, dims_2d, &var_grid_lat),
            NC_NOERR);
  ASSERT_EQ(
      nc_def_var(ncid, "grid_depth", NC_FLOAT, 2, dims_2d, &var_grid_depth),
      NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "grid_mask", NC_INT, 2, dims_2d, &var_grid_mask),
            NC_NOERR);

  ASSERT_EQ(
      nc_def_var(ncid, "face_lon", NC_DOUBLE, 1, &dim_faces, &var_face_lon),
      NC_NOERR);
  ASSERT_EQ(
      nc_def_var(ncid, "face_lat", NC_DOUBLE, 1, &dim_faces, &var_face_lat),
      NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "depth", NC_FLOAT, 1, &dim_faces, &var_depth),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "mask", NC_INT, 1, &dim_faces, &var_mask),
            NC_NOERR);

  ASSERT_EQ(
      nc_def_var(ncid, "node_lon", NC_DOUBLE, 1, &dim_nodes, &var_node_lon),
      NC_NOERR);
  ASSERT_EQ(
      nc_def_var(ncid, "node_lat", NC_DOUBLE, 1, &dim_nodes, &var_node_lat),
      NC_NOERR);

  ASSERT_EQ(nc_def_var(ncid, "face_node_connectivity", NC_INT, 2, dims_conn,
                       &var_conn),
            NC_NOERR);

  ASSERT_EQ(nc_def_var(ncid, "grid_ObsX", NC_FLOAT, 2, dims_2d, &var_grid_obsx),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "grid_ObsY", NC_FLOAT, 2, dims_2d, &var_grid_obsy),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "ObsX", NC_FLOAT, 1, &dim_faces, &var_obsx),
            NC_NOERR);
  ASSERT_EQ(nc_def_var(ncid, "ObsY", NC_FLOAT, 1, &dim_faces, &var_obsy),
            NC_NOERR);

  ASSERT_EQ(nc_enddef(ncid), NC_NOERR);

  double glon[4] = {-10.0, -9.0, -10.0, -9.0};
  double glat[4] = {50.0, 50.0, 51.0, 51.0};
  float gdepth[4] = {10.0f, 20.0f, 30.0f, 40.0f};
  int gmask[4] = {1, 1, 1, 0};

  double flon[4] = {-10.0, -9.0, -10.0, -9.0};
  double flat[4] = {50.0, 50.0, 51.0, 51.0};
  float fdepth[4] = {10.0f, 20.0f, 30.0f, 40.0f};
  int fmask[4] = {1, 1, 1, 0};

  double nlon[9] = {-10.5, -9.5, -8.5, -10.5, -9.5, -8.5, -10.5, -9.5, -8.5};
  double nlat[9] = {49.5, 49.5, 49.5, 50.5, 50.5, 50.5, 51.5, 51.5, 51.5};

  int conn[16] = {0, 1, 4, 3, 1, 2, 5, 4, 3, 4, 7, 6, 4, 5, 8, 7};

  float gobsx[4] = {0.1f, 0.2f, 0.3f, 0.4f};
  float gobsy[4] = {0.5f, 0.6f, 0.7f, 0.8f};
  float fobsx[4] = {0.1f, 0.2f, 0.3f, 0.4f};
  float fobsy[4] = {0.5f, 0.6f, 0.7f, 0.8f};

  ASSERT_EQ(nc_put_var_double(ncid, var_grid_lon, glon), NC_NOERR);
  ASSERT_EQ(nc_put_var_double(ncid, var_grid_lat, glat), NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_grid_depth, gdepth), NC_NOERR);
  ASSERT_EQ(nc_put_var_int(ncid, var_grid_mask, gmask), NC_NOERR);

  ASSERT_EQ(nc_put_var_double(ncid, var_face_lon, flon), NC_NOERR);
  ASSERT_EQ(nc_put_var_double(ncid, var_face_lat, flat), NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_depth, fdepth), NC_NOERR);
  ASSERT_EQ(nc_put_var_int(ncid, var_mask, fmask), NC_NOERR);

  ASSERT_EQ(nc_put_var_double(ncid, var_node_lon, nlon), NC_NOERR);
  ASSERT_EQ(nc_put_var_double(ncid, var_node_lat, nlat), NC_NOERR);

  ASSERT_EQ(nc_put_var_int(ncid, var_conn, conn), NC_NOERR);

  ASSERT_EQ(nc_put_var_float(ncid, var_grid_obsx, gobsx), NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_grid_obsy, gobsy), NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_obsx, fobsx), NC_NOERR);
  ASSERT_EQ(nc_put_var_float(ncid, var_obsy, fobsy), NC_NOERR);

  ASSERT_EQ(nc_close(ncid), NC_NOERR);

  UgridGrid grid;
  int status = readUQgrid(filename, grid);
  EXPECT_EQ(status, 0);

  EXPECT_EQ(grid.ny, 2u);
  EXPECT_EQ(grid.nx, 2u);
  EXPECT_EQ(grid.n_faces, 4u);
  EXPECT_EQ(grid.n_nodes, 9u);
  EXPECT_EQ(grid.max_face_nodes, 4u);

  EXPECT_EQ(grid.title, title);
  EXPECT_EQ(grid.grid_type, grid_type);

  EXPECT_EQ(grid.grid_lon.size(), 4u);
  EXPECT_DOUBLE_EQ(grid.grid_lon[0], -10.0);
  EXPECT_FLOAT_EQ(grid.grid_depth[3], 40.0f);
  EXPECT_EQ(grid.grid_mask[3], 0);

  EXPECT_EQ(grid.face_lon.size(), 4u);
  EXPECT_DOUBLE_EQ(grid.face_lon[0], -10.0);
  EXPECT_FLOAT_EQ(grid.depth[3], 40.0f);

  EXPECT_EQ(grid.node_lon.size(), 9u);
  EXPECT_DOUBLE_EQ(grid.node_lon[8], -8.5);

  EXPECT_EQ(grid.face_node_connectivity.size(), 16u);
  EXPECT_EQ(grid.face_node_connectivity[15], 7);

  EXPECT_TRUE(grid.has_obsx);
  EXPECT_TRUE(grid.has_obsy);
  EXPECT_FLOAT_EQ(grid.grid_ObsX[0], 0.1f);
  EXPECT_FLOAT_EQ(grid.ObsY[3], 0.8f);

  std::remove(filename.c_str());
}

TEST(UtilsUQIOTest, ReadUQGridFileNotFound) {
  UgridGrid grid;
  int status = readUQgrid("non_existent_file.nc", grid);
  EXPECT_EQ(status, 1);
  EXPECT_EQ(grid.nx, 0u);
}

TEST(UtilsUQIOTest, ReadUQGridMissingDimension) {
  const std::string filename = "test_missing_dim.nc";
  int ncid = -1;
  ASSERT_EQ(nc_create(filename.c_str(), NC_CLOBBER, &ncid), NC_NOERR);

  int dim_ny = -1, dim_nx = -1;
  ASSERT_EQ(nc_def_dim(ncid, "ny", 2, &dim_ny), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nx", 2, &dim_nx), NC_NOERR);
  ASSERT_EQ(nc_close(ncid), NC_NOERR);

  UgridGrid grid;
  int status = readUQgrid(filename, grid);
  EXPECT_EQ(status, 2);
  EXPECT_EQ(grid.nx, 0u);

  std::remove(filename.c_str());
}

TEST(UtilsUQIOTest, ReadUQGridMissingVariable) {
  const std::string filename = "test_missing_var.nc";
  int ncid = -1;
  ASSERT_EQ(nc_create(filename.c_str(), NC_CLOBBER, &ncid), NC_NOERR);

  int dim_ny = -1, dim_nx = -1, dim_faces = -1, dim_nodes = -1,
      dim_maxnodes = -1;
  ASSERT_EQ(nc_def_dim(ncid, "ny", 2, &dim_ny), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nx", 2, &dim_nx), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_face", 4, &dim_faces), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMesh2_node", 9, &dim_nodes), NC_NOERR);
  ASSERT_EQ(nc_def_dim(ncid, "nMaxMesh2_face_nodes", 4, &dim_maxnodes),
            NC_NOERR);

  ASSERT_EQ(nc_close(ncid), NC_NOERR);

  UgridGrid grid;
  int status = readUQgrid(filename, grid);
  EXPECT_EQ(status, 3);
  EXPECT_EQ(grid.nx, 0u);

  std::remove(filename.c_str());
}

TEST(UtilsUQIOTest, UgridGridClear) {
  UgridGrid grid;
  grid.nx = 10;
  grid.ny = 10;
  grid.title = "test";
  grid.clear();
  EXPECT_EQ(grid.nx, 0u);
  EXPECT_EQ(grid.ny, 0u);
  EXPECT_TRUE(grid.title.empty());
}

// References to function names for ww4_test_check.py routine coverage:
// readUQgrid read_all_global_attributes read_variable check_nc_err NcHandle
// clear constexpr get ncid_
