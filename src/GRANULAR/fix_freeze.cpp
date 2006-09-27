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

#include "string.h"
#include "fix_freeze.h"
#include "atom.h"
#include "modify.h"
#include "comm.h"
#include "error.h"

/* ---------------------------------------------------------------------- */

FixFreeze::FixFreeze(int narg, char **arg) : Fix(narg, arg)
{
  if (narg != 3) error->all("Illegal fix freeze command");

  if (atom->check_style("granular") == 0)
    error->all("Must use fix freeze with atom style granular");
}

/* ---------------------------------------------------------------------- */

int FixFreeze::setmask()
{
  int mask = 0;
  mask |= POST_FORCE;
  return mask;
}

/* ---------------------------------------------------------------------- */

void FixFreeze::init()
{
  // error if more than one freeze fix

  int count = 0;
  for (int i = 0; i < modify->nfix; i++)
    if (strcmp(modify->fix[i]->style,"freeze") == 0) count++;
  if (count > 1) error->all("More than one freeze fix");
}

/* ---------------------------------------------------------------------- */

void FixFreeze::setup()
{
  post_force(1);
}

/* ---------------------------------------------------------------------- */

void FixFreeze::post_force(int vflag)
{
  double **f = atom->f;
  double **phia = atom->phia;
  int *mask = atom->mask;
  int nlocal = atom->nlocal;

  for (int i = 0; i < nlocal; i++)
    if (mask[i] & groupbit) {
      f[i][0] = 0.0;
      f[i][1] = 0.0;
      f[i][2] = 0.0;
      phia[i][0] = 0.0;
      phia[i][1] = 0.0;
      phia[i][2] = 0.0;
    }
}
