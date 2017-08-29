/*~--------------------------------------------------------------------------~*
 *  @@@@@@@@  @@           @@@@@@   @@@@@@@@ @@
 * /@@/////  /@@          @@////@@ @@////// /@@
 * /@@       /@@  @@@@@  @@    // /@@       /@@
 * /@@@@@@@  /@@ @@///@@/@@       /@@@@@@@@@/@@
 * /@@////   /@@/@@@@@@@/@@       ////////@@/@@
 * /@@       /@@/@@//// //@@    @@       /@@/@@
 * /@@       @@@//@@@@@@ //@@@@@@  @@@@@@@@ /@@
 * //       ///  //////   //////  ////////  //
 *
 * Copyright (c) 2016 Los Alamos National Laboratory, LLC
 * All rights reserved
 *~--------------------------------------------------------------------------~*/

#ifndef flecsi_mpi_dense_h
#define flecsi_mpi_dense_h

//----------------------------------------------------------------------------//
// POLICY_NAMESPACE must be defined before including storage_type.h!!!
// Using this approach allows us to have only one storage_type_t
// definintion that can be used by all data policies -> code reuse...
#define POLICY_NAMESPACE mpi
#include "flecsi/data/storage_type.h"
#undef POLICY_NAMESPACE
//----------------------------------------------------------------------------//

#include "flecsi/data/common/data_types.h"
#include "flecsi/data/common/privilege.h"
#include "flecsi/data/data_client.h"
#include "flecsi/data/data_handle.h"
#include "flecsi/execution/context.h"
#include "flecsi/utils/const_string.h"
#include "flecsi/utils/index_space.h"

#include <algorithm>
#include <memory>

///
/// \file
/// \date Initial file creation: Apr 7, 2016
///

namespace flecsi {
namespace data {
namespace mpi {

//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=//
// Helper type definitions.
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=//

//----------------------------------------------------------------------------//
// Dense handle.
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Dense accessor.
//----------------------------------------------------------------------------//

///
/// \brief dense_accessor_t provides logically array-based access to data
///        variables that have been registered in the data model.
///
/// \tparam T The type of the data variable. If this type is not
///           consistent with the type used to register the data, bad things
///           can happen. However, it can be useful to reinterpret the type,
///           e.g., when writing raw bytes. This class is part of the
///           low-level \e flecsi interface, so it is assumed that you
///           know what you are doing...
///
template<
  typename T,
  size_t EP,
  size_t SP,
  size_t GP
>
struct dense_handle_t : public data_handle__<T, EP, SP, GP>
{
  //--------------------------------------------------------------------------//
  // Type definitions.
  //--------------------------------------------------------------------------//

  using base = data_handle__<T, EP, SP, GP>;

  //--------------------------------------------------------------------------//
  // Constructors.
  //--------------------------------------------------------------------------//

  // FIXME: calling to base class constructor?
  ///
  /// Default constructor.
  ///
  dense_handle_t() {}

	///
  /// Copy constructor.
	///
//	dense_handle_t(
//    const dense_handle_t & a
//  )
//  :
//    label_(a.label_)
//  {}

  template<size_t EP2, size_t SP2, size_t GP2>
  dense_handle_t(const dense_handle_t<T, EP2, SP2, GP2> & h)
    : base(reinterpret_cast<const base&>(h)),
      label_(h.label_)
  {}

  //--------------------------------------------------------------------------//
  // Member data interface.
  //--------------------------------------------------------------------------//

	///
  /// \brief Return a std::string containing the label of the data variable
  ///       reference by this accessor.
	///
  const std::string &
  label() const
  {
    return label_;
  } // label

	///
  /// \brief Return the index space size of the data variable
  ///        referenced by this accessor.
	///
  size_t
  size() const
  {
    return base::combined_size;
  } // size

  ///
  // \brief Return the index space size of the data variable
  //        referenced by this handle.
  ///
  size_t
  exclusive_size() const
  {
    return base::exclusive_size;
  } // size

  ///
  // \brief Return the index space size of the data variable
  //        referenced by this handle.
  ///
  size_t
  shared_size() const
  {
    return base::shared_size;
  } // size

  ///
  // \brief Return the index space size of the data variable
  //        referenced by this handle.
  ///
  size_t
  ghost_size() const
  {
    return base::ghost_size;
  } // size

  //--------------------------------------------------------------------------//
  // Operators.
  //--------------------------------------------------------------------------//

	///
  /// \brief Provide logical array-based access to the data for this
  ///        data variable.  This is the const operator version.
  ///
  /// \tparam E A complex index type.
  ///
  /// This version of the operator is provided to support use with
  /// \e flecsi mesh entity types \ref mesh_entity_base_t.
	///
  template<typename E>
  const T &
  operator [] (
    E * e
  ) const
  {
    return this->operator[](e->template id<0>());
  } // operator []

	///
  /// \brief Provide logical array-based access to the data for this
  ///        data variable.  This is the const operator version.
  ///
  /// \tparam E A complex index type.
  ///
  /// This version of the operator is provided to support use with
  /// \e flecsi mesh entity types \ref mesh_entity_base_t.
	///
  template<typename E>
  T &
  operator [] (
    E * e
  )
  {
    return this->operator[](e->template id<0>());
  } // operator []

	///
  /// \brief Provide logical array-based access to the data for this
  ///        data variable.  This is the const operator version.
  ///
  /// \tparam E A complex index type.
  ///
  /// This version of the operator is provided to support use with
  /// \e flecsi mesh entity types \ref mesh_entity_base_t.
	///
  template<typename E>
  const T &
  operator () (
    E * e
  ) const
  {
    return this->operator[](e->template id<0>());
  } // operator []

	///
  /// \brief Provide logical array-based access to the data for this
  ///        data variable.  This is the const operator version.
  ///
  /// \tparam E A complex index type.
  ///
  /// This version of the operator is provided to support use with
  /// \e flecsi mesh entity types \ref mesh_entity_base_t.
	///
  template<typename E>
  T &
  operator () (
    E * e
  )
  {
    return this->operator[](e->template id<0>());
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  const T &
  operator [] (
    size_t index
  ) const
  {
    assert(index < base::combined_size && "index out of range");
    return base::combined_data[index];
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  T &
  operator [] (
    size_t index
  )
  {
    assert(index < base::combined_size && "index out of range");
    return base::combined_data[index];
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  const T &
  exclusive (
    size_t index
  ) const
  {
    assert(index < base::exclusive_size && "index out of range");
    return base::exclusive_data[index];
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  T &
  exclusive (
    size_t index
  )
  {
    assert(index < base::exclusive_size && "index out of range");
    return base::exclusive_data[index];
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  const T &
  shared (
    size_t index
  ) const
  {
    assert(index < base::shared_size && "index out of range");
    return base::shared_data[index];
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  T &
  shared (
    size_t index
  )
  {
    assert(index < base::shared_size && "index out of range");
    return base::shared_data[index];
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  const T &
  ghost (
    size_t index
  ) const
  {
    assert(index < base::ghost_size && "index out of range");
    return base::ghost_data[index];
  } // operator []

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  T &
  ghost (
    size_t index
  )
  {
    assert(index < base::ghost_size && "index out of range");
    return base::ghost_data[index];
  } // operator []

//  ///
//  // \brief Provide logical array-based access to the data for this
//  //        data variable.  This is the const operator version.
//  //
//  // \tparam E A complex index type.
//  //
//  // This version of the operator is provided to support use with
//  // \e flecsi mesh entity types \ref mesh_entity_base_t.
//  ///
//  template<typename E>
//  const T &
//  operator () (
//    E * e
//  ) const
//  {
//    return this->operator()(e->template id<0>());
//  } // operator ()
//
//  ///
//  // \brief Provide logical array-based access to the data for this
//  //        data variable.  This is the const operator version.
//  //
//  // \tparam E A complex index type.
//  //
//  // This version of the operator is provided to support use with
//  // \e flecsi mesh entity types \ref mesh_entity_base_t.
//  ///
//  template<typename E>
//  T &
//  operator () (
//    E * e
//  )
//  {
//    return this->operator()(e->template id<0>());
//  } // operator ()

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  const T &
  operator () (
    size_t index
  ) const
  {
    assert(index < base::combined_size && "index out of range");
    return base::combined_data[index];
  } // operator ()

  ///
  // \brief Provide logical array-based access to the data for this
  //        data variable.  This is the const operator version.
  //
  // \param index The index of the data variable to return.
  ///
  T &
  operator () (
    size_t index
  )
  {
    assert(index < base::combined_size && "index out of range");
    return base::combined_data[index];
  } // operator ()

	///
  /// \brief Test to see if this accessor is empty
  ///
  /// \return true if registered.
  ///
  operator bool() const
  {
    return base::primary_data != nullptr;
  } // operator bool

  template<typename, size_t, size_t, size_t>
  friend class dense_handle_t;

private:
  std::string label_ = "";
}; // struct dense_handle_t

//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=//
// Main type definition.
//+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=//

//----------------------------------------------------------------------------//
// Dense storage type.
//----------------------------------------------------------------------------//

///
/// FIXME: Dense storage type.
///
template<>
struct storage_type__<dense>
{
  //--------------------------------------------------------------------------//
  // Type definitions.
  //--------------------------------------------------------------------------//

  template<
    typename T,
    size_t EP,
    size_t SP,
    size_t GP
  >
  using handle_t = dense_handle_t<T, EP, SP, GP>;

  template<
    typename DATA_CLIENT_TYPE,
    typename DATA_TYPE,
    size_t NAMESPACE,
    size_t NAME,
    size_t VERSION
  >
  static
  handle_t<DATA_TYPE, 0, 0, 0>
  get_handle(
    const data_client_t & data_client
  )
  {
    handle_t<DATA_TYPE, 0, 0, 0> h;

    auto& context = execution::context_t::instance();
  
    using client_type = typename DATA_CLIENT_TYPE::type_identifier_t;

    // get field_info for this data handle
    auto& field_info =
      context.get_field_info(
        typeid(typename DATA_CLIENT_TYPE::type_identifier_t).hash_code(),
      utils::hash::field_hash<NAMESPACE, NAME>(VERSION));

    // get color_info for this field.
    auto& color_info = (context.coloring_info(field_info.index_space)).at(context.color());

    auto& registered_field_data = context.registered_field_data();
    auto fieldDataIter = registered_field_data.find(field_info.fid);
    if (fieldDataIter == registered_field_data.end()) {
      size_t size = field_info.size * (color_info.exclusive +
                                       color_info.shared +
                                       color_info.ghost);
      // TODO: deal with VERSION
      execution::context_t::instance().register_field_data(field_info.fid,
                                                           size);
    }

    auto data = registered_field_data[field_info.fid].data();
    // populate data member of data_handle_t
    auto &hb = dynamic_cast<data_handle__<DATA_TYPE, 0, 0, 0>&>(h);

    hb.fid = field_info.fid;
    hb.index_space = field_info.index_space;
    hb.data_client_hash = field_info.data_client_hash;

    hb.exclusive_size = color_info.exclusive;
    hb.combined_data = hb.exclusive_buf = hb.exclusive_data =
      reinterpret_cast<DATA_TYPE *>(data);
    hb.combined_size = color_info.exclusive;

    hb.shared_size = color_info.shared;
    hb.shared_data = hb.shared_buf = hb.exclusive_data + hb.exclusive_size;
    hb.combined_size += color_info.shared;

    hb.ghost_size = color_info.ghost;
    hb.ghost_data = hb.ghost_buf = hb.shared_data + hb.shared_size;
    hb.combined_size += color_info.ghost;

    return h;
  }

}; // struct storage_type_t

} // namespace mpi
} // namespace data
} // namespace flecsi

#endif // flecsi_mpi_dense_h

/*~-------------------------------------------------------------------------~-*
 * Formatting options
 * vim: set tabstop=2 shiftwidth=2 expandtab :
 *~-------------------------------------------------------------------------~-*/
