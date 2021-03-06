/*
    @@@@@@@@  @@           @@@@@@   @@@@@@@@ @@
   /@@/////  /@@          @@////@@ @@////// /@@
   /@@       /@@  @@@@@  @@    // /@@       /@@
   /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@
   /@@////   /@@/@@@@@@@/@@       ////////@@/@@
   /@@       /@@/@@//// //@@    @@       /@@/@@
   /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@
   //       ///  //////   //////  ////////  //

   Copyright (c) 2016, Los Alamos National Security, LLC
   All rights reserved.
                                                                              */
#pragma once

/*! @file */

#include <functional>
#include <memory>

namespace flecsi {
namespace execution {

//----------------------------------------------------------------------------//
// Future concept.
//----------------------------------------------------------------------------//

/*!
 Abstract interface type for MPI futures.

 @ingroup legion-execution
 */

template<typename R, launch_type_t launch = launch_type_t::single>
struct mpi_future_u {
  using result_t = R;

  /*!
    wait() method
   */
  void wait() {}

  /*!
    get() mothod
   */
  const result_t & get(size_t index = 0) const {
    return result_;
  }

  // private:

  /*!
    set method
   */
  void set(const result_t & result) {
    result_ = result;
  }

  operator R &() {
    return result_;
  }

  operator const R &() const {
    return result_;
  }

  result_t result_;

}; // struct mpi_future_u

/*!
 FIXME documentation
 */
template<launch_type_t launch>
struct mpi_future_u<void, launch> {
  /*!
   FIXME documentation
   */
  void wait() {}

}; // struct mpi_future_u

template<typename RETURN, launch_type_t launch>
using flecsi_future = mpi_future_u<RETURN, launch_type_t::single>;

} // namespace execution
} // namespace flecsi
