// ---------------------------------------------------------------------
// $Id: 3d_refinement_01.cc 31349 2013-10-20 19:07:06Z maier $
//
// Copyright (C) 2008 - 2013, 2015 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the deal.II distribution.
//
// ---------------------------------------------------------------------


// create a shared tria mesh and refine it

#include "../tests.h"
#include <deal.II/base/logstream.h>
#include <deal.II/base/tensor.h>
#include <deal.II/grid/tria.h>
#include <deal.II/distributed/shared_tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/numerics/data_out.h>

#include <fstream>

template <int dim, int spacedim>
void write_mesh (const parallel::shared::Triangulation<dim,spacedim> &tria,
                 const char                                *filename_)
{
  DataOut<dim> data_out;
  data_out.attach_triangulation (tria);
  Vector<float> subdomain (tria.n_active_cells());
  for (unsigned int i=0; i<subdomain.size(); ++i)
    subdomain(i) = tria.locally_owned_subdomain();
  data_out.add_data_vector (subdomain, "subdomain");

  data_out.build_patches ();
  const std::string filename = (filename_ +
                                Utilities::int_to_string
                                (tria.locally_owned_subdomain(), 4));
  {
    std::ofstream output ((filename + ".vtu").c_str());
    data_out.write_vtu (output);
  }
}



template<int dim>
void test()
{
  parallel::shared::Triangulation<dim> tr(MPI_COMM_WORLD);


  GridGenerator::hyper_cube(tr);
  tr.begin_active()->set_refine_flag();
  tr.execute_coarsening_and_refinement ();
  tr.begin_active()->set_refine_flag();
  tr.execute_coarsening_and_refinement ();

  deallog
      << " locally_owned_subdomain(): " << tr.locally_owned_subdomain() << "\n"
      << " n_active_cells: " << tr.n_active_cells() << "\n"
      << " n_levels: " << tr.n_levels() << "\n"
      << " n_global_levels: " << tr.n_global_levels()  << "\n"
      //<< " n_locally_owned_active_cells: " << tr.n_locally_owned_active_cells() << "\n"
      //<< " n_global_active_cells: " << tr.n_global_active_cells() << "\n"
      << std::endl;

  /*deallog << "n_locally_owned_active_cells_per_processor: ";
  std::vector<unsigned int> v = tr.n_locally_owned_active_cells_per_processor();
  for (unsigned int i=0;i<v.size();++i)
    deallog << v[i] << " ";
    deallog << std::endl;*/

  deallog << "subdomains: ";
  typename  parallel::shared::Triangulation<dim>::active_cell_iterator it=tr.begin_active();
  for (; it!=tr.end(); ++it)
    {
      deallog << it->subdomain_id() << " ";
    }
  deallog << std::endl;

  //write_mesh(tr, "mesh");
}


int main(int argc, char *argv[])
{
  Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
  MPILogInitAll all;

  deallog.push("2d");
  test<2>();
  deallog.pop();
  deallog.push("3d");
  test<3>();
  deallog.pop();
}
