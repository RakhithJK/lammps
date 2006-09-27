/* ----------------------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   www.cs.sandia.gov/~sjplimp/lammps.html
   Steve Plimpton, sjplimp@sandia.gov, Sandia National Laboratories

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under 
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#ifndef PAIR_EAM_ALLOY_H
#define PAIR_EAM_ALLOY_H

#include "pair_eam.h"

class PairEAMAlloy : public PairEAM {
 public:
  PairEAMAlloy();
  void coeff(int, char **);
  double init_one(int, int);
  void init_style();

 private:
  int read_setfl(char *, int, int);
  void store_setfl();
  void interpolate();
};

#endif
