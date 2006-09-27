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

#include "math.h"
#include "stdlib.h"
#include "angle_cosine.h"
#include "atom.h"
#include "neighbor.h"
#include "domain.h"
#include "comm.h"
#include "force.h"
#include "memory.h"
#include "error.h"

#define SMALL 0.001

/* ----------------------------------------------------------------------
   free all arrays 
------------------------------------------------------------------------- */

AngleCosine::~AngleCosine()
{
  if (allocated) {
    memory->sfree(setflag);
    memory->sfree(k);
  }
}

/* ---------------------------------------------------------------------- */

void AngleCosine::compute(int eflag, int vflag)
{
  int i1,i2,i3,n,type,factor;
  double delx1,dely1,delz1,delx2,dely2,delz2,rfactor;
  double rsq1,rsq2,r1,r2,c,a,a11,a12,a22,vx1,vx2,vy1,vy2,vz1,vz2;

  energy = 0.0;
  if (vflag) for (n = 0; n < 6; n++) virial[n] = 0.0;

  double **x = atom->x;
  double **f = atom->f;
  int **anglelist = neighbor->anglelist;
  int nanglelist = neighbor->nanglelist;
  int nlocal = atom->nlocal;
  int newton_bond = force->newton_bond;

  for (n = 0; n < nanglelist; n++) {

    i1 = anglelist[n][0];
    i2 = anglelist[n][1];
    i3 = anglelist[n][2];
    type = anglelist[n][3];

    if (newton_bond) factor = 3;
    else {
      factor = 0;
      if (i1 < nlocal) factor++;
      if (i2 < nlocal) factor++;
      if (i3 < nlocal) factor++;
      }
    rfactor = factor/3.0;

    // 1st bond

    delx1 = x[i1][0] - x[i2][0];
    dely1 = x[i1][1] - x[i2][1];
    delz1 = x[i1][2] - x[i2][2];
    domain->minimum_image(&delx1,&dely1,&delz1);

    rsq1 = delx1*delx1 + dely1*dely1 + delz1*delz1;
    r1 = sqrt(rsq1);

    // 2nd bond

    delx2 = x[i3][0] - x[i2][0];
    dely2 = x[i3][1] - x[i2][1];
    delz2 = x[i3][2] - x[i2][2];
    domain->minimum_image(&delx2,&dely2,&delz2);

    rsq2 = delx2*delx2 + dely2*dely2 + delz2*delz2;
    r2 = sqrt(rsq2);

    // c = cosine of angle

    c = delx1*delx2 + dely1*dely2 + delz1*delz2;
    c /= r1*r2;
    if (c > 1.0) c = 1.0;
    if (c < -1.0) c = -1.0;

    // force & energy

    if (eflag) energy += rfactor * k[type]*(1.0+c);

    a = -k[type];
        
    a11 = a*c / rsq1;
    a12 = -a / (r1*r2);
    a22 = a*c / rsq2;
        
    vx1 = a11*delx1 + a12*delx2;
    vx2 = a22*delx2 + a12*delx1;
    vy1 = a11*dely1 + a12*dely2;
    vy2 = a22*dely2 + a12*dely1;
    vz1 = a11*delz1 + a12*delz2;
    vz2 = a22*delz2 + a12*delz1;

    // apply force to each of 3 atoms

    if (newton_bond || i1 < nlocal) {
      f[i1][0] -= vx1;
      f[i1][1] -= vy1;
      f[i1][2] -= vz1;
    }

    if (newton_bond || i2 < nlocal) {
      f[i2][0] += vx1 + vx2;
      f[i2][1] += vy1 + vy2;
      f[i2][2] += vz1 + vz2;
    }

    if (newton_bond || i3 < nlocal) {
      f[i3][0] -= vx2;
      f[i3][1] -= vy2;
      f[i3][2] -= vz2;
    }

    // virial contribution

    if (vflag) {
      virial[0] -= rfactor * (delx1*vx1 + delx2*vx2);
      virial[1] -= rfactor * (dely1*vy1 + dely2*vy2);
      virial[2] -= rfactor * (delz1*vz1 + delz2*vz2);
      virial[3] -= rfactor * (delx1*vy1 + delx2*vy2);
      virial[4] -= rfactor * (delx1*vz1 + delx2*vz2);
      virial[5] -= rfactor * (dely1*vz1 + dely2*vz2);
    }
  }
}

/* ---------------------------------------------------------------------- */

void AngleCosine::allocate()
{
  allocated = 1;
  int n = atom->nangletypes;

  k = (double *) memory->smalloc((n+1)*sizeof(double),"angle:k");
  setflag = (int *) memory->smalloc((n+1)*sizeof(int),"angle:setflag");
  for (int i = 1; i <= n; i++) setflag[i] = 0;
}

/* ----------------------------------------------------------------------
   set coeffs for one type
------------------------------------------------------------------------- */

void AngleCosine::coeff(int which, int narg, char **arg)
{
  if (which != 0) error->all("Invalid coeffs for this angle style");
  if (narg != 2) error->all("Incorrect args for angle coefficients");
  if (!allocated) allocate();

  int ilo,ihi;
  force->bounds(arg[0],atom->nangletypes,ilo,ihi);

  double k_one = atof(arg[1]);

  int count = 0;
  for (int i = ilo; i <= ihi; i++) {
    k[i] = k_one;
    setflag[i] = 1;
    count++;
  }

  if (count == 0) error->all("Incorrect args for angle coefficients");
}

/* ---------------------------------------------------------------------- */

double AngleCosine::equilibrium_angle(int i)
{
  return PI;
}

/* ----------------------------------------------------------------------
   proc 0 writes out coeffs to restart file 
------------------------------------------------------------------------- */

void AngleCosine::write_restart(FILE *fp)
{
  fwrite(&k[1],sizeof(double),atom->nangletypes,fp);
}

/* ----------------------------------------------------------------------
   proc 0 reads coeffs from restart file, bcasts them 
------------------------------------------------------------------------- */

void AngleCosine::read_restart(FILE *fp)
{
  allocate();

  if (comm->me == 0) fread(&k[1],sizeof(double),atom->nangletypes,fp);
  MPI_Bcast(&k[1],atom->nangletypes,MPI_DOUBLE,0,world);

  for (int i = 1; i <= atom->nangletypes; i++) setflag[i] = 1;
}

/* ---------------------------------------------------------------------- */

double AngleCosine::single(int type, int i1, int i2, int i3, double rfactor)
{
  double **x = atom->x;

  double delx1 = x[i1][0] - x[i2][0];
  double dely1 = x[i1][1] - x[i2][1];
  double delz1 = x[i1][2] - x[i2][2];
  domain->minimum_image(&delx1,&dely1,&delz1);
  double r1 = sqrt(delx1*delx1 + dely1*dely1 + delz1*delz1);
  
  double delx2 = x[i3][0] - x[i2][0];
  double dely2 = x[i3][1] - x[i2][1];
  double delz2 = x[i3][2] - x[i2][2];
  domain->minimum_image(&delx2,&dely2,&delz2);
  double r2 = sqrt(delx2*delx2 + dely2*dely2 + delz2*delz2);

  double c = delx1*delx2 + dely1*dely2 + delz1*delz2;
  c /= r1*r2;
  if (c > 1.0) c = 1.0;
  if (c < -1.0) c = -1.0;

  return (rfactor * k[type]*(1.0+c));
}
