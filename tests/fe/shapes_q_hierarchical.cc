// ---------------------------------------------------------------------
//
// Copyright (C) 2013 - 2014 by the deal.II authors
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


#include "../tests.h"
#include "shapes.h"
#include <deal.II/fe/fe_q_hierarchical.h>
#include <deal.II/fe/mapping_q1.h>
#include <fstream>
#include <string>

#define PRECISION 8


template<int dim>
void plot_FE_Q_Hierarchical_shape_functions()
{
  MappingQGeneric<dim> m(1);

  FE_Q_Hierarchical<dim> q1(1);
  plot_shape_functions(m, q1, "QHierarchical1");
  plot_face_shape_functions(m, q1, "QHierarchical1");
  test_compute_functions(m, q1, "QHierarchical1");

  FE_Q_Hierarchical<dim> q2(2);
  plot_shape_functions(m, q2, "QHierarchical2");
  plot_face_shape_functions(m, q2, "QHierarchical2");
  test_compute_functions(m, q2, "QHierarchical2");

  // skip the following tests to
  // reduce run-time
  if (dim < 3)
    {
      FE_Q_Hierarchical<dim> q3(3);
      plot_shape_functions(m, q3, "QHierarchical3");
      plot_face_shape_functions(m, q3, "QHierarchical3");
      test_compute_functions(m, q3, "QHierarchical3");

      FE_Q_Hierarchical<dim> q4(4);
      plot_shape_functions(m, q4, "QHierarchical4");
      plot_face_shape_functions(m, q4, "QHierarchical4");
      test_compute_functions(m, q4, "QHierarchical4");
    }
}


int
main()
{
  std::ofstream logfile ("output");
  deallog << std::setprecision(PRECISION) << std::fixed;
  deallog.attach(logfile);
  deallog.depth_console(0);
  deallog.threshold_double(1.e-10);

  plot_FE_Q_Hierarchical_shape_functions<1>();
  plot_FE_Q_Hierarchical_shape_functions<2>();
  plot_FE_Q_Hierarchical_shape_functions<3>();

  return 0;
}
