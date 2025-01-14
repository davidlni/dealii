// ---------------------------------------------------------------------
//
// Copyright (C) 2005 - 2015 by the deal.II authors
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



// Test GridGenerator::extrude. 2d meshes are always correctly
// edge-oriented, and so if we stack them one on top of the other, we
// should also get a 3d mesh for which both edge and face orientations
// are correct -- or so I thought, but this turns out to not be true,
// see https://github.com/dealii/dealii/issues/1013
//
// test this for a circle extruded to a cylinder

#include "../tests.h"
#include <deal.II/base/logstream.h>
#include <deal.II/base/tensor.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_out.h>
#include <deal.II/fe/fe_q.h>

#include <fstream>
#include <iomanip>


void test(std::ostream &out)
{
  Triangulation<2> tr;
  GridGenerator::hyper_ball (tr);

  for (Triangulation<2>::active_cell_iterator c=tr.begin_active();
       c!=tr.end(); ++c)
    {
      deallog << "2d cell " << c << " has the following face orientations:"
              << std::endl;
      for (unsigned int l=0; l<GeometryInfo<2>::faces_per_cell; ++l)
        deallog << "    " << (c->face_orientation(l) ? "true" : "false")
                << std::endl;
    }

  Triangulation<3> tr3;
  GridGenerator::extrude_triangulation(tr, 2, 1.0, tr3);

  for (Triangulation<3>::active_cell_iterator c=tr3.begin_active();
       c!=tr3.end(); ++c)
    {
      deallog << "3d cell " << c << " has the following face orientation/flips and edge orientations:"
              << std::endl;
      for (unsigned int f=0; f<GeometryInfo<3>::faces_per_cell; ++f)
        deallog << "    face=" << f
                << (c->face_orientation(f) ? " -> true" : " -> false")
                << (c->face_flip(f) ? "/true" : "/false")
                << std::endl;
      for (unsigned int e=0; e<GeometryInfo<3>::lines_per_cell; ++e)
        deallog << "    edge=" << e << (c->line_orientation(e) ? " -> true" : " -> false")
                << std::endl;
    }
}


int main()
{
  std::ofstream logfile("output");
  deallog.attach(logfile);
  deallog.depth_console(0);
  deallog.threshold_double(1.e-10);

  test(logfile);
}
