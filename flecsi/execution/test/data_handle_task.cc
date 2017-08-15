/*~-------------------------------------------------------------------------~~*
 * Copyright (c) 2014 Los Alamos National Security, LLC
 * All rights reserved.
 *~-------------------------------------------------------------------------~~*/

///
/// \file
/// \date Initial file creation: Apr 11, 2017
///

#define DH2

#include <cinchtest.h>

#include "flecsi/execution/execution.h"
#include "flecsi/io/simple_definition.h"
#include "flecsi/coloring/dcrs_utils.h"
#include "flecsi/data/data.h"
#include "flecsi/supplemental/coloring/add_colorings.h"
#include "flecsi/supplemental/mesh/empty_mesh_2d.h"

using namespace flecsi;
using namespace supplemental;

clog_register_tag(coloring);

#if FLECSI_RUNTIME_MODEL == FLECSI_RUNTIME_MODEL_legion
template<typename T, size_t EP, size_t SP, size_t GP>
using handle_t =
  data::legion::dense_handle_t<T, EP, SP, GP>;
#elif FLECSI_RUNTIME_MODEL == FLECSI_RUNTIME_MODEL_mpi
template<typename T, size_t EP, size_t SP, size_t GP>
using handle_t =
  data::mpi::dense_handle_t<T, EP, SP, GP>;
#endif

void task1(handle_t<double, ro, ro, ro> x, double y) {
  //np(y);
} // task1

void data_handle_dump(handle_t<double, rw, ro, ro> x) {
  clog(info) << "label: " << x.label() << std::endl;
  clog(info) << "combined size: " << x.size() << std::endl;
  clog(info) << "exclusive size: " << x.exclusive_size() << std::endl;
  clog(info) << "shared size: " << x.shared_size() << std::endl;
  clog(info) << "ghost size: " << x.ghost_size() << std::endl;
}

void exclusive_writer(handle_t<double, wo, ro, ro> x) {
  clog(info) << "exclusive writer write" << std::endl;
  for (int i = 0; i < x.exclusive_size(); i++) {
    x.exclusive(i) = static_cast<double>(i);
  }
}

void exclusive_reader(handle_t<double, ro, ro, ro> x) {
  clog(info) << "exclusive reader read: " << std::endl;
  for (int i = 0; i < x.exclusive_size(); i++) {
    ASSERT_EQ(x.exclusive(i), static_cast<double>(i));
  }
}

flecsi_register_task(task1, loc, single);
flecsi_register_task(data_handle_dump, loc, single);
flecsi_register_task(exclusive_writer, loc, single);
flecsi_register_task(exclusive_reader, loc, single);

flecsi_register_field(empty_mesh_2d_t, ns, pressure, double, dense, 1, 0);

namespace flecsi {
namespace execution {

//----------------------------------------------------------------------------//
// Specialization driver.
//----------------------------------------------------------------------------//

void specialization_tlt_init(int argc, char ** argv) {
  clog(info) << "In specialization top-level-task init" << std::endl;

  coloring_map_t map;
  map.vertices = 1;
  map.cells = 0;

  flecsi_execute_mpi_task(add_colorings, map);
} // specialization_tlt_init

//----------------------------------------------------------------------------//
// User driver.
//----------------------------------------------------------------------------//

void driver(int argc, char ** argv) {
  clog(info) << "In driver" << std::endl;

  int rank, size;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  auto ch = flecsi_get_client_handle(empty_mesh_2d_t, meshes, mesh1);

  auto h = flecsi_get_handle(ch, ns, pressure, double, dense, 0);

//  flecsi_execute_task(task1, single, h, 128);
  flecsi_execute_task(data_handle_dump, single, h);
  flecsi_execute_task(exclusive_writer, single, h);
  flecsi_execute_task(exclusive_reader, single, h);
} // specialization_driver

//----------------------------------------------------------------------------//
// TEST.
//----------------------------------------------------------------------------//

TEST(data_handle, testname) {
  
} // TEST

} // namespace execution
} // namespace flecsi

/*~------------------------------------------------------------------------~--*
 * Formatting options for vim.
 * vim: set tabstop=2 shiftwidth=2 expandtab :
 *~------------------------------------------------------------------------~--*/

#undef DH2
