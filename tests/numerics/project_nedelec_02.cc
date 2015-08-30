// ---------------------------------------------------------------------
//
// Copyright (C) 2006 - 2014 by the deal.II authors
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



// check that VectorTools::project works for Nedelec elements correctly

char logname[] = "output";


#include "project_common.h"


template <int dim>
void test ()
{
  if (dim > 1)
    // only p=1 implemented at present
    for (unsigned int p=0; p<2; ++p)
      test_with_hanging_nodes (FE_Nedelec<dim>(p), p, 1);
}