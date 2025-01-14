// ---------------------------------------------------------------------
//
// Copyright (C) 2008 - 2014 by the deal.II authors
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



// test distributed solution_transfer with one processor

#include "../tests.h"
#include "coarse_grid_common.h"
#include <deal.II/base/logstream.h>
#include <deal.II/base/tensor.h>
#include <deal.II/grid/tria.h>
#include <deal.II/distributed/tria.h>
#include <deal.II/distributed/solution_transfer.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/grid/grid_in.h>
#include <deal.II/grid/intergrid_map.h>

#include <deal.II/lac/petsc_vector.h>
#include <deal.II/fe/fe_q.h>

#include <deal.II/numerics/vector_tools.h>
#include <deal.II/fe/mapping_q1.h>
#include <deal.II/base/function.h>

#include <fstream>
#include <cstdlib>

template<int dim>
class MyFunction : public Function<dim>
{
public:
  MyFunction () : Function<dim>() {};

  virtual double value (const Point<dim>   &p,
                        const unsigned int) const
  {
    double f=p[0]*2.0+1.0;
    if (dim>1)
      f*=p[1]*3.3-1.0;
    if (dim>2)
      f*=p[2]*5.0;
    return f;
  };
};

template<int dim>
void test(std::ostream & /*out*/)
{
  MyFunction<dim> func;
  MappingQGeneric<dim> mapping(1);
  parallel::distributed::Triangulation<dim> tr(MPI_COMM_WORLD);

  GridGenerator::hyper_cube(tr);
  tr.refine_global (2);
  DoFHandler<dim> dofh(tr);
  static const FE_Q<dim> fe(1);
  dofh.distribute_dofs (fe);

  parallel::distributed::SolutionTransfer<dim, Vector<double> > soltrans(dofh);

  for (typename Triangulation<dim>::active_cell_iterator
       cell = tr.begin_active();
       cell != tr.end(); ++cell)
    {
      cell->set_refine_flag();
    }

  tr.prepare_coarsening_and_refinement();

  Vector<double> solution(dofh.n_dofs());
  VectorTools::interpolate (mapping,
                            * static_cast<dealii::DoFHandler<dim>* >(&dofh),
                            func,
                            solution);

  soltrans.prepare_for_coarsening_and_refinement(solution);

  tr.execute_coarsening_and_refinement ();

  dofh.distribute_dofs (fe);

  Vector<double> interpolated_solution(dofh.n_dofs());
  soltrans.interpolate(interpolated_solution);

  deallog << "norm: " << interpolated_solution.l2_norm() << std::endl;
  Vector<double> difference(tr.n_global_active_cells());

  VectorTools::integrate_difference(mapping,
                                    dofh,
                                    interpolated_solution,
                                    func,
                                    difference,
                                    QGauss<dim>(2),
                                    VectorTools::L2_norm);
  deallog << "error: " << difference.l2_norm() << std::endl;

}


int main(int argc, char *argv[])
{
#ifdef DEAL_II_WITH_MPI
  Utilities::MPI::MPI_InitFinalize mpi_initialization (argc, argv, 1);
#else
  (void)argc;
  (void)argv;
#endif

  std::ofstream logfile("output");
  deallog.attach(logfile);
  deallog.depth_console(0);
  deallog.threshold_double(1.e-10);

  deallog.push("2d");
  test<2>(logfile);
  deallog.pop();
  deallog.push("3d");
  test<3>(logfile);
  deallog.pop();


}
