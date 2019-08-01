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
#pragma once

/*! @file */

#if !defined(__FLECSI_PRIVATE__)
#error Do not include this file directly!
#endif

#include <flecsi/execution/context.hh>
#include <flecsi/utils/flog.hh>
#include <flecsi/utils/mpi_type_traits.hh>

#include <type_traits>

flog_register_tag(reduction_wrapper);

namespace flecsi {
namespace execution {
namespace mpi {

template<size_t HASH, typename TYPE>
struct reduction_wrapper_u {

  using rhs_t = typename TYPE::RHS;
  using lhs_t = typename TYPE::LHS;

  // MPI does not have support for mixed-type reductions
  static_assert(std::is_same_v<lhs_t, rhs_t>, "type mismatch: LHS != RHS");

  /*!
    Wrapper to convert the type-erased MPI function to the typed C++ method.
   */

  static void
  mpi_wrapper(void * in, void * inout, int * len, MPI_Datatype * dptr) {

    lhs_t * lhs = reinterpret_cast<lhs_t *>(inout);
    rhs_t * rhs = reinterpret_cast<rhs_t *>(in);

    for(size_t i{0}; i < *len; ++i) {
      TYPE::template apply<true>(lhs[i], rhs[i]);
    } // for
  } // mpi_wrapper

  /*!
    Register the user-defined reduction operator with the runtime.
   */

  static void registration_callback() {
    {
      flog_tag_guard(reduction_wrapper);
      flog(info) << "Executing reduction wrapper callback for " << HASH
                 << std::endl;
    } // scope

    // Get the runtime context
    auto & context_ = context_t::instance();

    // Get a reference to the operator map
    auto & reduction_ops = context_.reduction_operations();

    // Check if operator has already been registered
    flog_assert(reduction_ops.find(HASH) == reduction_ops.end(),
      typeid(TYPE).name() << " has already been registered with this name");

    // Create the MPI data type if it isn't P.O.D.
    if constexpr(!std::is_pod_v<lhs_t>) {
      // Get the datatype map from the context
      auto & reduction_types = context_.reduction_types();

      // Get a hash from the runtime type information
      size_t typehash = typeid(lhs_t).hash_code();

      // Search for this type...
      auto dtype = reduction_types.find(typehash);

      if(dtype == reduction_types.end()) {
        // Add the MPI type if it doesn't exist
        MPI_Datatype datatype;
        constexpr size_t datatype_size = sizeof(typename TYPE::RHS);
        MPI_Type_contiguous(datatype_size, MPI_BYTE, &datatype);
        MPI_Type_commit(&datatype);
        reduction_types[typehash] = datatype;
      } // if
    } // if

    // Create the operator and register it with the runtime
    MPI_Op mpiop;
    MPI_Op_create(mpi_wrapper, true, &mpiop);
    reduction_ops[HASH] = mpiop;
  } // registration_callback

}; // struct reduction_wrapper_u

} // namespace mpi
} // namespace execution
} // namespace flecsi