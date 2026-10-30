/**
 *       +--------------------------------------------------------+
 *       | WAVEWATCH IV, open source, code management by NOAA/NWS |
 *       +--------------------------------------------------------+
 *
 * @file test_grid_helper.h
 * @brief Helper routine to generate a minimal valid NetCDF-UGRID grid file for
 * unit tests.
 * @copyright © 2026 National Weather Service, National Oceanic and Atmospheric
 * Administration. WAVEWATCH IV (TM) and WW4 (TM) are trademarks of the National
 * Weather Service.
 * @author Main Author(s): Aldgisl (AI Persona), Hendrik L. Tolman
 * @author Contributors: Jules (Agentic AI)
 * @date Initial, 2026-10-30
 * @date Last update : 2026-10-30
 */

#pragma once

#include <iostream>
#include <netcdf.h>
#include <string>

namespace ww4_core::test_helpers {

inline void createDummyUgridFile(const std::string &filename) {
  int ncid = -1;
  int st = nc_create(filename.c_str(), NC_CLOBBER, &ncid);
  if (st != NC_NOERR) {
    std::cerr << "createDummyUgridFile nc_create failed: " << nc_strerror(st)
              << std::endl;
    return;
  }

  int dim_ny = -1, dim_nx = -1, dim_faces = -1, dim_nodes = -1,
      dim_maxnodes = -1;
  nc_def_dim(ncid, "ny", 2, &dim_ny);
  nc_def_dim(ncid, "nx", 2, &dim_nx);
  nc_def_dim(ncid, "nMesh2_face", 4, &dim_faces);
  nc_def_dim(ncid, "nMesh2_node", 9, &dim_nodes);
  nc_def_dim(ncid, "nMaxMesh2_face_nodes", 4, &dim_maxnodes);

  const std::string title = "Test Grid";
  nc_put_att_text(ncid, NC_GLOBAL, "title", title.size(), title.c_str());

  int dims_2d[2] = {dim_ny, dim_nx};
  int dims_conn[2] = {dim_faces, dim_maxnodes};

  int var_grid_lon = -1, var_grid_lat = -1, var_grid_depth = -1,
      var_grid_mask = -1;
  int var_face_lon = -1, var_face_lat = -1, var_depth = -1, var_mask = -1;
  int var_node_lon = -1, var_node_lat = -1, var_conn = -1;

  nc_def_var(ncid, "grid_lon", NC_DOUBLE, 2, dims_2d, &var_grid_lon);
  nc_def_var(ncid, "grid_lat", NC_DOUBLE, 2, dims_2d, &var_grid_lat);
  nc_def_var(ncid, "grid_depth", NC_FLOAT, 2, dims_2d, &var_grid_depth);
  nc_def_var(ncid, "grid_mask", NC_INT, 2, dims_2d, &var_grid_mask);

  nc_def_var(ncid, "face_lon", NC_DOUBLE, 1, &dim_faces, &var_face_lon);
  nc_def_var(ncid, "face_lat", NC_DOUBLE, 1, &dim_faces, &var_face_lat);
  nc_def_var(ncid, "depth", NC_FLOAT, 1, &dim_faces, &var_depth);
  nc_def_var(ncid, "mask", NC_INT, 1, &dim_faces, &var_mask);

  nc_def_var(ncid, "node_lon", NC_DOUBLE, 1, &dim_nodes, &var_node_lon);
  nc_def_var(ncid, "node_lat", NC_DOUBLE, 1, &dim_nodes, &var_node_lat);

  nc_def_var(ncid, "face_node_connectivity", NC_INT, 2, dims_conn, &var_conn);

  nc_enddef(ncid);

  double glon[4] = {-10.0, -9.0, -10.0, -9.0};
  double glat[4] = {50.0, 50.0, 51.0, 51.0};
  float gdepth[4] = {10.0f, 20.0f, 30.0f, 40.0f};
  int gmask[4] = {1, 1, 1, 0};

  double nlon[9] = {0.0};
  double nlat[9] = {0.0};
  int conn[16] = {0};

  nc_put_var_double(ncid, var_grid_lon, glon);
  nc_put_var_double(ncid, var_grid_lat, glat);
  nc_put_var_float(ncid, var_grid_depth, gdepth);
  nc_put_var_int(ncid, var_grid_mask, gmask);

  nc_put_var_double(ncid, var_face_lon, glon);
  nc_put_var_double(ncid, var_face_lat, glat);
  nc_put_var_float(ncid, var_depth, gdepth);
  nc_put_var_int(ncid, var_mask, gmask);

  nc_put_var_double(ncid, var_node_lon, nlon);
  nc_put_var_double(ncid, var_node_lat, nlat);

  nc_put_var_int(ncid, var_conn, conn);

  nc_close(ncid);
}

} // namespace ww4_core::test_helpers
