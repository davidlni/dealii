// ---------------------------------------------------------------------
//
// Copyright (C) 2003 - 2015 by the deal.II authors
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


// check the correctness of fe_values.shape_gradient for FE_Q by comparing the
// integral of all shape gradients with the flux over the boundary by the
// divergence theorem

#include "../tests.h"
#include <deal.II/base/logstream.h>
#include <deal.II/base/function.h>
#include <deal.II/base/quadrature_lib.h>
#include <deal.II/lac/vector.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/tria_boundary_lib.h>
#include <deal.II/dofs/dof_handler.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_raviart_thomas.h>
#include <deal.II/fe/fe_nedelec.h>
#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_q1.h>

#include <sstream>
#include <fstream>



template<int dim>
Tensor<1,dim> ones ()
{
  Tensor<1,dim> result;
  for (unsigned int i=0; i<dim; ++i)
    result[i] = 1.0;
  return result;
}

template<int dim>
void test (const Triangulation<dim> &tr,
           const FiniteElement<dim> &fe,
           const double tolerance)
{
  DoFHandler<dim> dof(tr);
  dof.distribute_dofs(fe);

  std::stringstream ss;

  deallog << "FE=" << fe.get_name() << std::endl;

  const QGauss<dim> quadrature(3);
  FEValues<dim> fe_values (fe, quadrature, update_gradients
                           | update_quadrature_points
                           | update_JxW_values);

  const QGauss<dim-1> face_quadrature(3);
  FEFaceValues<dim> fe_face_values (fe, face_quadrature,
                                    update_values
                                    | update_quadrature_points
                                    | update_normal_vectors
                                    | update_JxW_values);

  for (typename DoFHandler<dim>::active_cell_iterator cell = dof.begin_active();
       cell != dof.end();
       ++cell)
    {
      fe_values.reinit (cell);

      deallog << "Cell nodes:" << std::endl;
      for (unsigned int i=0; i<GeometryInfo<dim>::vertices_per_cell; ++i)
        {
          deallog << i << ": ( ";
          for (unsigned int d=0; d<dim; ++d)
            deallog << cell->vertex(i)[d] << " ";
          deallog << ")" << std::endl;
        }

      bool cell_ok = true;

      for (unsigned int c=0; c<fe.n_components(); ++c)
        {
          FEValuesExtractors::Scalar single_component (c);

          for (unsigned int i=0; i<fe_values.dofs_per_cell; ++i)
            {
              ss << "component=" << c
                 << ", dof=" << i
                 << std::endl;

              Tensor<1,dim> bulk_integral;
              for (unsigned int q=0; q<fe_values.n_quadrature_points; ++q)
                {
                  bulk_integral += fe_values[single_component].gradient(i,q) * fe_values.JxW(q);
                }

              Tensor<1,dim> boundary_integral;
              for (unsigned int face=0; face<GeometryInfo<dim>::faces_per_cell; ++face)
                {
                  fe_face_values.reinit(cell,face);
                  for (unsigned int q=0; q<fe_face_values.n_quadrature_points; ++q)
                    {
                      boundary_integral += fe_face_values[single_component].value (i,q)
                                           * fe_face_values.normal_vector(q)
                                           * fe_face_values.JxW(q);
                    }
                }

              if ((bulk_integral-boundary_integral).norm_square() > tolerance * (bulk_integral.norm() + boundary_integral.norm()))
                {
                  deallog << "Failed:" << std::endl;
                  deallog << ss.str() << std::endl;
                  deallog << "    bulk integral=" << bulk_integral << std::endl;
                  deallog << "boundary integral=" << boundary_integral << std::endl;
                  deallog << "Error! difference between bulk and surface integrals is greater than " << tolerance << "!\n\n" << std::endl;
                  cell_ok = false;
                }

              ss.str("");
            }
        }

      deallog << (cell_ok? "OK: cell bulk and boundary integrals match...\n" : "Failed divergence test...\n") << std::endl;
    }
}



template<int dim>
void test_hyper_ball(const double tolerance)
{
  Triangulation<dim> tr;
  GridGenerator::hyper_ball(tr);

  static const HyperBallBoundary<dim> boundary;
  tr.set_boundary (0, boundary);

  tr.refine_global(1);

  FE_Q<dim> fe(2);
  test(tr, fe, tolerance);
}


int main()
{
  std::ofstream logfile ("output");
  deallog << std::setprecision (3);

  deallog.attach(logfile);
  deallog.depth_console (0);
  deallog.threshold_double(1.e-7);

  test_hyper_ball<2>(1e-6);
  test_hyper_ball<3>(1e-6);

  deallog << "done..." << std::endl;
}
