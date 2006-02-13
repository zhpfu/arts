/* Copyright (C) 2006 Claudia Emde <claudia@sat.physik.uni-bremen.de>

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
   USA. */

/**
 \file   disort.h
 \author Claudia Emde <claudia@sat.physik.uni-bremen.de>
 \date   Tue Feb  7 11:48:17 2006
  
 \brief  Functions for disort interface
 * 
 * 
 */



void dtauc_ssalbCalc(
                    VectorView dtauc,
                    VectorView ssalb,
                    const Agenda& opt_prop_part_agenda,
                    const Agenda& scalar_gas_absorption_agenda,
                    const Agenda& spt_calc_agenda,
                    ConstTensor4View pnd_field,
                    const ArrayOfIndex& cloudbox_limits,
                    ConstTensor3View t_field,
                    ConstTensor3View z_field, 
                    ConstVectorView p_grid,
                    ConstTensor4View vmr_field
                    );

void phase_functionCalc(//Output
                       MatrixView phase_function,
                       //Input
                       const ArrayOfSingleScatteringData& scat_data_mono, 
                       ConstTensor4View pnd_field
                       );

void pmomCalc(//Output
              MatrixView pmom,
              //Input
              ConstMatrixView phase_function, 
              ConstVectorView scat_angle_grid,
              const Index n_legendre
              );