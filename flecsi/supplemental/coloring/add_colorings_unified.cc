#include <mpi.h>
#include <unistd.h>

#include <flecsi/supplemental/coloring/add_colorings_dependent_partition.h>
#include <flecsi/execution/legion/legion_tasks.h>
#include <flecsi/execution/legion/dependent_partition.h>
#include <flecsi/topology/mesh_definition.h>
#include <flecsi/io/simple_definition.h>


namespace flecsi {
namespace execution {

void
add_colorings_unified()    
{
  printf("start DP\n");
  flecsi::io::simple_definition_t sd("simple2d-8x8.msh");
  
  int num_cells = sd.num_entities(1);
  int num_vertices = sd.num_entities(0);

  legion_dependent_partition legion_dp;
  
  dependent_partition &dp = legion_dp;
  
  legion_entity cells = dp.load_entity(num_cells, 0, 2, sd);
  
  legion_adjacency cell_to_cell = dp.load_cell_to_entity(cells, cells, sd);
  
  legion_partition cell_primary = dp.partition_by_color(cells);
  
  legion_partition cell_closure = dp.partition_by_image(cells, cells, cell_to_cell, cell_primary);
  
  legion_partition cell_ghost = dp.partition_by_difference(cells, cell_closure, cell_primary);
  
  legion_partition cell_ghost_closure = dp.partition_by_image(cells, cells, cell_to_cell, cell_ghost);
  
  legion_partition cell_shared = dp.partition_by_intersection(cells, cell_ghost_closure, cell_primary);
  
  legion_partition cell_exclusive = dp.partition_by_difference(cells, cell_primary, cell_shared);
  
  legion_entity vertices = dp.load_entity(num_vertices, 1, 2, sd);
  
  legion_adjacency cell_to_vertex = dp.load_cell_to_entity(cells, vertices, sd);
  
  legion_partition vertex_alias = dp.partition_by_image(cells, vertices, cell_to_vertex, cell_primary);
  
  dp.min_reduction_by_color(vertices, vertex_alias);
  
  legion_partition vertex_primary = dp.partition_by_color(vertices);
  
  legion_partition vertex_of_ghost_cell = dp.partition_by_image(cells, vertices, cell_to_vertex, cell_ghost);
  
  legion_partition vertex_ghost = dp.partition_by_difference(vertices, vertex_of_ghost_cell, vertex_primary);
  
  legion_partition vertex_of_shared_cell = dp.partition_by_image(cells, vertices, cell_to_vertex, cell_shared);
  
  legion_partition vertex_shared = dp.partition_by_intersection(vertices, vertex_of_shared_cell, vertex_primary);
  
  legion_partition vertex_exclusive = dp.partition_by_difference(vertices, vertex_primary, vertex_shared);
  
  /*
  legion_entity edges = dp.load_entity(num_vertices, 2, 3);
  
  legion_adjacency cell_to_edge = dp.load_cell_to_entity(cells, edges);
  
  legion_partition edge_alias = dp.partition_by_image(cells, edges, cell_to_edge, cell_primary);
  
  dp.min_reduction_by_color(edges, edge_alias);
  
  legion_partition edge_primary = dp.partition_by_color(edges);
  
  legion_partition edge_of_ghost_cell = dp.partition_by_image(cells, edges, cell_to_edge, cell_ghost);
  
  legion_partition edge_ghost = dp.partition_by_difference(edges, edge_of_ghost_cell, edge_primary);
  
  legion_partition edge_of_shared_cell = dp.partition_by_image(cells, edges, cell_to_edge, cell_shared);
  
  legion_partition edge_shared = dp.partition_by_intersection(edges, edge_of_shared_cell, edge_primary);
  
  legion_partition edge_exclusive = dp.partition_by_difference(edges, edge_primary, edge_shared); */
  
  dp.output_partition(cells, cell_primary, cell_ghost, cell_shared, cell_exclusive);
  
  dp.output_partition(vertices, vertex_primary, vertex_ghost, vertex_shared, vertex_exclusive);
  
//  dp.output_partition(edges, edge_primary, edge_ghost, edge_shared, edge_exclusive);
  
}

} // namespace execution
} // namespace flecsi