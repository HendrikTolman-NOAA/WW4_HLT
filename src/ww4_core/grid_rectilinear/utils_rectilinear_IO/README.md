<p align="center">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/WW4_banner.jpg" alt="WW4 banner" height="100">
</p>

# <p align="center"> WAVEWATCH IV<sup> TM</sup> (WW4<sup> TM</sup>) Rectilinear Grid I/O </p>

*Last updated: November 23, 2026*

This directory contains the NetCDF I/O routines for rectilinear grids in WAVEWATCH IV (namespace `ww4::io`). This document lists all data and metadata that is read from and written to the NetCDF files. All routines use the netcdf-c API, and all file handles are closed through the RAII wrapper `NcHandle`.

Two file types are handled:

| File type | Read by | Written by | Contents |
|-----------|---------|------------|----------|
| Grid file (NetCDF-UGRID 1.0) | `readRectilinearGrid` | `writeRectilinearGrid` | Grid metadata, 2D grid fields, 1D UGRID mesh fields |
| Partition map file (NetCDF) | `readRectilinearPartitionMapNetCDF` | `openRectilinearPartitionMapFile`, `writeSingleRectilinearPartitionMap`, `closeRectilinearPartitionMapFile`, `writeRectilinearPartitionMapNetCDF` | Grid metadata, number of partitions, one or more 2D partition maps |

## Files in this directory

| File | Purpose |
|------|---------|
| `rectilinear_read_grid.h/.cpp` | `UgridGrid` container, `readRectilinearGrid`, `printGridMetadata` |
| `rectilinear_write_grid.h/.cpp` | `writeRectilinearGrid` |
| `rectilinear_read_partition.h/.cpp` | `PartitionMapData` container, `readRectilinearPartitionMapNetCDF` |
| `rectilinear_write_partition.h/.cpp` | Open, append, close, and one-shot partition map writers |
| `rectilinear_nc_handle.h` | RAII wrapper for the NetCDF `ncid` |
| `test_ugrid_helper.h` | Helper that creates minimal UGRID files for tests |

The `*PartitionMapNetCDF` and `*PartitionMapFile` names without `Rectilinear` in the name (for example `readPartitionMapNetCDF`) are inline backward-compatible wrappers.

---

## 1. Grid file

### 1.1 Dimensions

| Dimension | Container field | Meaning | Written value (if container field is 0) |
|-----------|-----------------|---------|------------------------------------------|
| `ny` | `ny` | Number of grid rows | Required, must be > 0 |
| `nx` | `nx` | Number of grid columns | Required, must be > 0 |
| `nMesh2_face` | `n_faces` | Number of mesh faces (cells) | `nx * ny` |
| `nMesh2_node` | `n_nodes` | Number of mesh nodes | `(nx + 1) * (ny + 1)` |
| `nMaxMesh2_face_nodes` | `max_face_nodes` | Maximum nodes per face | `4` |

All five dimensions are required when reading. If any is missing, the reader returns status 2.

### 1.2 Global attributes

The reader stores every global attribute of NetCDF type `NC_CHAR` in `UgridGrid::global_attrs` (trailing NUL characters and spaces are removed). Attributes of other types are ignored. The following attributes are also copied into dedicated fields.

| Attribute | Container field | Description |
|-----------|-----------------|-------------|
| `title` | `title` | Title of the grid |
| `source` | `source` | Source of the grid |
| `grid_type` | `grid_type` | Grid type |
| `coord_type` | `coord_type` | Coordinate type |
| `global_closure` | `global_closure` | Closure of the grid in the global domain |
| `bathy_source` | `bathy_source` | Source of the bathymetry |
| `bound_source` | `bound_source` | Source of the boundary (coastline) data |
| `Conventions` | `conventions` | Metadata conventions |
| `history` | `history` | Creation history |

Missing attributes are returned as empty strings. The reader does not fail on missing attributes.

The writer writes the attributes above, skipping any with an empty value, and then writes all other entries of `global_attrs` as text attributes. Entries in `global_attrs` that duplicate one of the named attributes are ignored, so the dedicated field takes precedence. The writer does not add or modify any attribute automatically (for example, it does not update `history`).

### 1.3 Variables

Row-major storage is used throughout. For 2D fields the index of the cell in row `j` and column `i` is `j * nx + i`. Face `f` of the 1D fields corresponds to the 2D cell with the same index when `n_faces = nx * ny`.

#### 2D grid fields, dimensions `(ny, nx)`

| Variable | NetCDF type | Container field | Required on read | Description |
|----------|-------------|-----------------|------------------|-------------|
| `grid_lon` | `double` | `grid_lon` | Yes | Longitude of the cell center (degrees east) |
| `grid_lat` | `double` | `grid_lat` | Yes | Latitude of the cell center (degrees north) |
| `grid_depth` | `float` | `grid_depth` | Yes | Bathymetric depth (m) |
| `grid_mask` | `int` | `grid_mask` | Yes | Point status mask |
| `grid_ObsX` | `float` | `grid_ObsX` | No | Subgrid obstruction in x direction |
| `grid_ObsY` | `float` | `grid_ObsY` | No | Subgrid obstruction in y direction |

#### 1D face fields, dimension `(nMesh2_face)`

| Variable | NetCDF type | Container field | Required on read | Description |
|----------|-------------|-----------------|------------------|-------------|
| `face_lon` | `double` | `face_lon` | Yes | Longitude of the face center (degrees east) |
| `face_lat` | `double` | `face_lat` | Yes | Latitude of the face center (degrees north) |
| `depth` | `float` | `depth` | Yes | Bathymetric depth of the face (m) |
| `mask` | `int` | `mask` | Yes | Point status mask of the face |
| `ObsX` | `float` | `ObsX` | No | Subgrid obstruction in x direction |
| `ObsY` | `float` | `ObsY` | No | Subgrid obstruction in y direction |

#### 1D node fields, dimension `(nMesh2_node)`

| Variable | NetCDF type | Container field | Required on read | Description |
|----------|-------------|-----------------|------------------|-------------|
| `node_lon` | `double` | `node_lon` | Yes | Longitude of the mesh node (degrees east) |
| `node_lat` | `double` | `node_lat` | Yes | Latitude of the mesh node (degrees north) |

#### Connectivity, dimensions `(nMesh2_face, nMaxMesh2_face_nodes)`

| Variable | NetCDF type | Container field | Required on read | Description |
|----------|-------------|-----------------|------------------|-------------|
| `face_node_connectivity` | `int` | `face_node_connectivity` | Yes | Zero-based indices of the nodes of each face |

The default connectivity generated by the writer (see 1.5) lists the four corner nodes of cell `(j, i)` in the order `(j, i)`, `(j, i+1)`, `(j+1, i+1)`, `(j+1, i)`, with node index `j * (nx + 1) + i`.

#### Mask values

The mask values interpreted by `printGridMetadata` are:

| Value | Meaning |
|-------|---------|
| 0 | Land point |
| 1 | Sea point |
| 2 | Boundary point |
| 3 | Excluded point |

### 1.4 Reading

`readRectilinearGrid(filepath, grid)` clears `grid` and returns one of the following status codes. On any non-zero code, `grid` is cleared again.

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | File does not exist or cannot be opened |
| 2 | One of the dimensions in 1.1 is missing |
| 3 | A required variable in 1.3 is missing or cannot be read |

Further read behavior:

- The obstruction fields are optional. `has_obsx` is set only if both `grid_ObsX` and `ObsX` can be read. `has_obsy` is set only if both `grid_ObsY` and `ObsY` can be read. Otherwise the corresponding vectors are left empty and the flag is false.
- Integer variables of type `NC_INT64` are converted to 32-bit integers. Other integer types are read through `nc_get_var_int`.
- Floating point variables are converted by netcdf-c to the container type.
- Variable attributes (such as `units` or `standard_name`) are not read.
- The sizes of the variables are taken from the dimensions. They are not checked against the dimension lengths in the file beyond what netcdf-c does.

`printGridMetadata(grid, os)` does not read or write a file. It prints the dimensions, the counts of land, sea, boundary, and excluded points (taken from `grid_mask`, or `mask` if `grid_mask` is empty), the obstruction status, and all non-empty named attributes in 1.2. It appends ` UTC` to `history` if the string contains none of `UTC`, `Z`, or `GMT`.

### 1.5 Writing

`writeRectilinearGrid(filepath, grid)` creates the file with `NC_CLOBBER`, which replaces an existing file. Missing parent directories are created.

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | `nx` or `ny` is 0, a 2D array (`grid_lon`, `grid_lat`, `grid_depth`, `grid_mask`) does not have `nx * ny` elements, or the file cannot be created |
| 2 | Failure to define dimensions, attributes, variables, or to end define mode |
| 3 | Failure to write variable data |

Default values used by the writer:

| Case | Behavior |
|------|----------|
| `n_faces`, `n_nodes`, `max_face_nodes` are 0 | Defaults in 1.1 are used |
| `face_lon`, `face_lat`, `depth`, `mask` do not have `n_faces` elements | The matching 2D array is written instead |
| `node_lon`, `node_lat` do not have `n_nodes` elements | Both are written as 0.0. Node coordinates are not derived from the 2D grid. |
| `face_node_connectivity` does not have `n_faces * max_face_nodes` elements | A quad connectivity is generated if `max_face_nodes == 4`, `n_faces == nx * ny`, and `n_nodes == (nx + 1) * (ny + 1)`. Otherwise all entries are 0. |
| Obstructions | `grid_ObsX` and `ObsX` are defined if `has_obsx` is true or `grid_ObsX` has `nx * ny` elements (same for Y). A missing array is written as 0.0. A missing 1D `ObsX` or `ObsY` is taken from the 2D array if available. |

The writer does not write variable attributes in the grid file.

---

## 2. Partition map file

### 2.1 Dimensions

| Dimension | Container field | Meaning | Required on read | Default if missing on read |
|-----------|-----------------|---------|------------------|-----------------------------|
| `ny` | `ny` | Number of grid rows | Yes | None |
| `nx` | `nx` | Number of grid columns | Yes | None |
| `nMesh2_face` | `n_faces` | Number of mesh faces | No | `ny * nx` |
| `nMesh2_node` | `n_nodes` | Number of mesh nodes | No | `(nx + 1) * (ny + 1)` |
| `nMaxMesh2_face_nodes` | `max_face_nodes` | Maximum nodes per face | No | `4` |
| `n_partition_maps` | `num_maps` | Number of partition maps (unlimited dimension) | No | `1` |

The writer defines all six dimensions. The mesh dimensions are taken from the grid, with the same defaults as in 1.1. They are stored as metadata only, since the file has no variables on these dimensions.

### 2.2 Global attributes

| Attribute | NetCDF type | Container field | Description |
|-----------|-------------|-----------------|-------------|
| `title`, `source`, `grid_type`, `coord_type`, `global_closure`, `bathy_source`, `bound_source`, `Conventions`, `history` | text | Same as in 1.2 (`PartitionMapData`) | Grid metadata copied from the `UgridGrid` passed to the writer |
| Other text attributes | text | `global_attrs` | Additional entries of `UgridGrid::global_attrs` |
| `num_partitions` | `int` | `num_partitions` | Total number of partitions. **Required on read.** |

As for the grid file, the reader stores all `NC_CHAR` global attributes in `global_attrs` and the writer skips empty values.

### 2.3 Variables

| Variable | NetCDF type | Dimensions | Container field | Description |
|----------|-------------|------------|-----------------|-------------|
| `partition_map` | `int` | `(n_partition_maps, ny, nx)` | `partition_map` | Partition ID of each grid cell |

Variable attributes written by the writer (not read):

| Attribute | Value |
|-----------|-------|
| `long_name` | `Domain partitioning map` |
| `_FillValue` | `0` (int) |

Because `_FillValue` is 0, any map record that was not written reads as all 0.

### 2.4 Reading

`readRectilinearPartitionMapNetCDF(filepath, part_data)` clears `part_data` and returns:

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | File does not exist or cannot be opened |
| 2 | `ny` or `nx` is missing, `ny`, `nx`, or `n_partition_maps` is 0, or the `num_partitions` attribute is missing |
| 3 | `partition_map` is missing, has a number of dimensions other than 2 or 3, or cannot be read |

Only one map is returned. For a 3D `partition_map`, this is the last record (index `n_partition_maps - 1`). A 2D `partition_map` is read as is. `num_maps` reports the number of records in the file.

### 2.5 Writing

The file is written in three steps (or in one step with `writeRectilinearPartitionMapNetCDF`, which writes a single map).

1. `openRectilinearPartitionMapFile(filepath, grid, num_partitions, ncid)` creates the file with `NC_CLOBBER`, creates missing parent directories, writes the dimensions in 2.1, the global attributes in 2.2, and defines `partition_map` with its attributes in 2.3. It returns the open `ncid` in data mode.
2. `writeSingleRectilinearPartitionMap(ncid, partition_map, map_index)` writes `ny * nx` values to record `map_index` and then increments `map_index`. Call this once per map.
3. `closeRectilinearPartitionMapFile(ncid)` closes the file.

| Routine | Codes |
|---------|-------|
| `openRectilinearPartitionMapFile` | 0 success, 1 file cannot be created, 2 `nx` or `ny` is 0, `num_partitions` is not positive, or a definition failure |
| `writeSingleRectilinearPartitionMap` | 0 success, 3 invalid `ncid`, missing variable or dimension, `partition_map` size not equal to `ny * nx`, or write failure |
| `closeRectilinearPartitionMapFile` | 0 success (also for a negative `ncid`), otherwise the netcdf-c error code |
| `writeRectilinearPartitionMapNetCDF` | Code of the first failing step, or 3 if only closing fails |

The partition file does not contain coordinates, depth, mask, or connectivity of the grid. Only the grid metadata and dimensions are copied from the `UgridGrid`.

---

## 3. Limitations

- Only global attributes of type `NC_CHAR` are read into containers. A numeric global attribute (other than `num_partitions`) is not preserved when a file is read and written again.
- Variable attributes are not read. Only `partition_map` has variable attributes written.
- The grid writer does not validate the contents of the 1D face, node, or connectivity arrays beyond their sizes.

#
<p align="right">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/noaa_logo.gif" alt="NOAA Logo" height="50" ; width="55">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/nws.jpg" alt="NWS Logo" height="50" width="50">
  <img src="https://github.com/NOAA-EMC/WW4/wiki/images/ncep_logo.gif" alt="NCEP Logo" height="50" width="75">
 </p>
