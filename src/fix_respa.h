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

#ifndef FIX_RESPA_H
#define FIX_RESPA_H

#include "fix.h"

class FixRespa : public Fix {
  friend class Respa;
  friend class FixShake;

 public:
  FixRespa(int, char **);
  ~FixRespa();
  int setmask();
  void init() {}

  int memory_usage();
  void grow_arrays(int);
  void copy_arrays(int, int);
  int pack_exchange(int, double *);
  int unpack_exchange(int, double *);

 private:
  int nlevels;
  double ***f_level;            // force at each rRESPA level
};

#endif
