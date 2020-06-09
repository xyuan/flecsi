/*
    @@@@@@@@  @@           @@@@@@   @@@@@@@@ @@
   /@@/////  /@@          @@////@@ @@////// /@@
   /@@       /@@  @@@@@  @@    // /@@       /@@
   /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@
   /@@////   /@@/@@@@@@@/@@       ////////@@/@@
   /@@       /@@/@@//// //@@    @@       /@@/@@
   /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@
   //       ///  //////   //////  ////////  //

   Copyright (c) 2016, Triad National Security, LLC
   All rights reserved.
                                                                              */

#include "flecsi/topo/unstructured/simple_definition.hh"
#include "flecsi/util/unit.hh"
#include <flecsi/execution.hh>

using namespace flecsi;

int
simple2d_16x16() {
  UNIT {
    topo::unstructured_impl::simple_definition sd("simple2d-16x16.msh");

    ASSERT_EQ(sd.dimension(), 2lu);
    ASSERT_EQ(sd.num_entities(0), 289lu);
    ASSERT_EQ(sd.num_entities(2), 256lu);
  };
}

int
simple_definition() {
  UNIT {
    // TODO: use test<> when reduction works for MPI tasks
    execute<simple2d_16x16, mpi>();
  };
} // simple_definition

flecsi::unit::driver<simple_definition> driver;
