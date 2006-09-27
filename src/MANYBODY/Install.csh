# Install/unInstall package classes in LAMMPS

# pair_eam.h must always be in src

if ($1 == 1) then

  cp style_manybody.h ..

  cp pair_eam.cpp ..
  cp pair_eam_alloy.cpp ..
  cp pair_eam_fs.cpp ..

#  cp pair_eam.h ..
  cp pair_eam_alloy.h ..
  cp pair_eam_fs.h ..

else if ($1 == 0) then

  rm ../style_manybody.h
  touch ../style_manybody.h

  rm ../pair_eam.cpp
  rm ../pair_eam_alloy.cpp
  rm ../pair_eam_fs.cpp

#  rm ../pair_eam.h
  rm ../pair_eam_alloy.h
  rm ../pair_eam_fs.h

endif
