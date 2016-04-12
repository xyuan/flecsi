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

#ifndef FLECSI_SAGITTARIUS_FIXTURE_H
#define FLECSI_SAGITTARIUS_FIXTURE_H

namespace flecsi {

class Sagittarius : public ::testing::Test {
protected:
  sagittarius_mesh_t <sagittarius_types> constellation;

  virtual void SetUp() override {
    // add vertices to the mesh
    std::vector<sagittarius_vertex_t *> vertices;
    for (size_t i = 0; i < 8; i++) {
      auto v = constellation.make<sagittarius_vertex_t>();
      constellation.add_entity<0, sagittarius_vertex_t::dimension>(v);
      vertices.push_back(v);
    }

    // add cells and cell to vertex connectivities to the mesh
    for (size_t i = 0; i < 2; i++) {
      auto cell = constellation.make<sagittarius_quad_t>();
      constellation.add_entity<2, 0>(cell);
      constellation.init_cell<0>(cell,
                                 {vertices[quads[i][0]],
                                  vertices[quads[i][1]],
                                  vertices[quads[i][2]],
                                  vertices[quads[i][3]]});
    }

    // actually compute connectivities between entities
    for (size_t i = 0; i < 2; i++) {
      auto cell = constellation.make<sagittarius_triangle_t>();
      constellation.add_entity<2, 0>(cell);
      constellation.init_cell<0>(cell,
                                 {vertices[triangles[i][0]],
                                  vertices[triangles[i][1]],
                                  vertices[triangles[i][2]]});
    }

    constellation.init();

    // convert and divide vertex to vertex and cell to cell connectivities into
    // two equal partitions in the form of Distributed CSR format as in ParMetis
    // manual.
    constellation.compute_graph_partition(0, 0, vertex_sizes, vertex_partitions);
    constellation.compute_graph_partition(0, 2, cell_sizes, cell_partitions);
  }

  virtual void TearDown() override { }

  std::vector<size_t> vertex_sizes = {4, 4};
  std::vector<mesh_graph_partition<size_t>> vertex_partitions;
  std::vector<size_t> cell_sizes = {2, 2};
  std::vector<mesh_graph_partition<size_t>> cell_partitions;

private:
  const int quads[2][4] = {
    {0, 1, 2, 3},
    {1, 4, 6, 2}
  };

  const int triangles[2][3] = {
    {4, 5, 6},
    {2, 6, 7}
  };
};
}
#endif //FLECSI_SAGITTARIUS_FIXTURE_H
