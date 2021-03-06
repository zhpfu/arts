!--------------------------------------------------------------------------------------------------------------------
module module_LLS
!--------------------------------------------------------------------------------------------------------------------
! This module contains all subroutines related to 
!
    interface  

        subroutine LLS_Matrix(j,k,h,molP,PerM,M, econ)
            use module_common_var
            use module_maths
            use module_phsub
            implicit none
            integer*8              , intent(in   ) :: j,k
            type (dta_SDF), pointer, intent(in   ) :: h
            type (dta_MOL)         , intent(in   ) :: molP, PerM
            type (dta_ERR)         , intent(inout) :: econ
            double precision       , intent(  out) :: M(4)
        end subroutine LLS_Matrix 

        subroutine LLS_AF_Matrix(j,k,h,molP,PerM,M, econ)
            use module_common_var
            use module_maths
            use module_phsub
            implicit none
            integer*8              , intent(in   ) :: j,k
            type (dta_SDF), pointer, intent(in   ) :: h
            type (dta_MOL)         , intent(in   ) :: molP, PerM
            type (dta_ERR)         , intent(inout) :: econ
            double precision       , intent(  out) :: M(12)
        end subroutine LLS_AF_Matrix      

    end interface

END module module_LLS
!--------------------------------------------------------------------------------------------------------------------
! MATHEMATIC FUNCTIONS AND SUBROUTINES ------------------------------------------------------------------------------
!--------------------------------------------------------------------------------------------------------------------
  SUBROUTINE LLS_Matrix(j,k,h,molP,PerM,M,econ)
!--------------------------------------------------------------------------------------------------------------------
! "LLS_Matrix": Subroutine that fills up the matrix of the LLS-problem for LLS-type: Linear and Model1.                   
!------------------------------------------ 
! vi := initial state (downwards transition j->k)
! CASE:  J(j) > J(k)
! M(j,k,:) = [ K1*Sum(K2(L)), 
!              -K1*Sum( K2(L) * (L*(L+1)) ), ...
!              -c2*K1*Sum( K2(L) * B0(L*(L+1)) ,...
!               K1*Sum(K2(L))];      
!------------------------------------------ 
    use module_common_var
    use module_maths
    use module_phsub
    IMPLICIT none
    ! a1, a2, a3, dc were declared in module_molecSP.SystemQparam:
    integer*8              , intent(in   ) :: j,k
    type (dta_SDF), pointer, intent(in   ) :: h
    type (dta_MOL)         , intent(in   ) :: molP, PerM
    type (dta_ERR)         , intent(inout) :: econ
    double precision       , intent(  out) :: M(4)
    !internal variables:
    integer*8             :: L, incr, step
    integer*8             :: li,lf
    integer*8             :: i, iniL,endL
    integer*8             :: Ni, Ni_p, Nf, Nf_p
    double precision      :: Ji, Ji_p, Jf, Jf_p 
    double precision      :: Si, Sf, sumK1, sumK2
    double precision      :: dE,Jaux, delta
    double precision      :: K1, K2
    double precision      :: AF1, AF2, m_4
    double precision      :: T, Ptot, RT
    !-----------------------------------------
      T    = molP % Temp
      Ptot = molP % Ptot
      RT   = T0/T
      AF1  = 1.0_dp
      AF2  = 1.0_dp
      sumK2= 0.d0
      sumK1= 0.d0
    !-----------------------------------------
    ! Since the molecules to be considered are linear -> symmetric 
    ! and the quadrupole - quadrupole interaction for the systems mol-X
    ! are meant to be dominant the interaction potential is likely 
    ! dominated by EVEN rank contributions. Then step = 2
    !
      step = 2
    ! 
    ! j -> Ji(j) > Ji(k)=Ji'
      Ji   = h%J(j,1); Ni = h%N(j,1); Si = h%espin(j,1)
      Jf   = h%J(j,2); Nf = h%N(j,2); Sf = h%espin(j,2)
    ! k-> 
      Ji_p = h%J(k,1); Ni_p = h%N(k,1)
      Jf_p = h%J(k,2); Nf_p = h%N(k,2)
    !
    ! li and lf are the vibrational angular momentum of the vibrational
    ! available for Linear molecules in HITRAN; 
    ! (due to its symmetry though).
    !
      li = h%lv2(1)
      lf = h%lv2(2) 
    ! 
    ! Selecting the initial and final L. Theoretically, is correct to 
    ! define L from 0 to inf (or the larger J in the band); however,
    ! is time comsuming and les efficient. 
    ! For further details, see Appendix A of [Niro et al. 2005] 
    !
      iniL=int(max(abs(Ji-Ji_p),abs(Jf-Jf_p)))
      endL=int(min((Ji+Ji_p),(Jf+Jf_p)))
      If( mod(iniL,step) .ne. 0 )iniL=iniL+1
    !
    ! Init. the LLS-P-Matrix to zero.
      M(1)=0.0_dp;M(2)=0.0_dp;M(3)=0.0_dp;M(4)=0.0_dp
    !
    ! Adiabatic factor 1:
      if (molP%M .eq. 7) then
          if (molP%AF_ON) then 
            AF1 = AFmol_X(molP, PerM, real(Ni,dp), step)
          endif
          K1 = Kpart1_O2(Ji, Ji_p, Jf, Jf_p, &
                         Ni, Ni_p, Nf, Nf_p, AF1,econ)
      else
          if (molP%AF_ON) then 
            AF1 = AFmol_X(molP, PerM, Ji, step)
          endif
          K1 = Kpart1(Ji_p, Jf, Jf_p, li, lf, AF1 )
      endif
      !
      do L = iniL,endL,step
        !
        ! _delta_Energy
        dE = abs( real(L*(L+1),dp) )
        !
        ! Adiabatic factor 2:
        if (molP%AF_ON) then
          AF2 = AFmol_X(molP, PerM, real(L,dp), step)
        endif
        if (molP%M .eq. 7) then
          K2 = Kpart2_O2(L, Ji, Ji_p, Jf, Jf_p, &
                         Ni, Ni_p, Nf, Nf_p, Si, Sf, AF2, econ)
        else
          K2 = Kpart2(L, Ji, Ji_p, Jf, Jf_p, li, lf, AF2,econ)
        endif
        !
        ! add to the M(j,:) vector
        M(1) = K2 + M(1)
        M(2) = K2*log(dE) + M(2)
        M(3) = K2*molP%B0*dE + M(3)
        !M(4) = K2 + M(4)
      enddo
      !
      ! DEFINITION OF M(j,:) VECTOR
      ! 
      Jaux = Ji
      if (Jaux .eq. 0) Jaux = 0.5_dp
      !
      ! Approximations:
      if (molP % LLSty .eq. "Linear") then
        !
        ! 1) Linear
        M(1) = K1 * M(1)
      else if (molP % LLSty .eq. "Model1") then 
        !
        ! 2) correction 1
        delta = dabs(h%Sig(j)-h%Sig(k))
        if (delta .lt. TOL) then
          call errorLines(delta,j,k,econ)
        endif
        M(1) = K1 * M(1)/( delta/molP%B0 )**(Ji_p/Jaux) 
      else
        call errorLLStyType(molP%LLSty,econ)
      endif
      M(2) = K1 * M(2)
      M(3) = K1 * (c2/T) * M(3)
      !M(4) = K1 * M(4)
      M(4) = M(1)
      !
  END SUBROUTINE LLS_Matrix
!--------------------------------------------------------------------------------------------------------------------
  SUBROUTINE LLS_AF_Matrix(j,k,h,molP,PerM,M,econ)
!--------------------------------------------------------------------------------------------------------------------
! "LLS_AF_Matrix": Subroutine that fills up the matrix of the extended LLS-problem 
!                  (the one that also takes into account the Adiabatic Factor,AF)                   
!------------------------------------------ 
!vi := initial state (downwards transition j->k)
!CASE:  J(j) > J(k)     
!------------------------------------------ 
    use module_common_var
    use module_maths
    use module_phsub
    IMPLICIT none
    ! a1, a2, a3, dc were declared in module_molecSP.SystemQparam:
    integer*8              , intent(in   ) :: j,k
    type (dta_SDF), pointer, intent(in   ) :: h
    type (dta_MOL)         , intent(in   ) :: molP, PerM
    type (dta_ERR)         , intent(inout) :: econ
    double precision       , intent(  out) :: M(12)
    !internal variables:
    integer*8             :: L, incr, step
    integer*8             :: li,lf
    integer*8             :: i, iniL,endL
    integer*8             :: Ni, Ni_p, Nf, Nf_p
    double precision      :: Ji, Ji_p, Jf, Jf_p 
    double precision      :: Si, Sf
    double precision      :: dE,Jaux, delta,coeffC
    double precision      :: K1, K2, AF1, AF2, m_4
    double precision      :: T, Ptot, RT
    !-----------------------------------------
      T    = molP % Temp
      Ptot = molP % Ptot
      RT   = T0/T
      AF1  = 1.0_dp
      AF2  = 1.0_dp
    !-----------------------------------------
    ! Since the molecules to be considered are linear -> symmetric 
    ! and the quadrupole - quadrupole interaction for the systems mol-X
    ! are meant to be dominant the interaction potential is likely 
    ! dominated by EVEN rank contributions. Then step = 2
    !
      step = 2
    ! 
    ! j -> Ji(j) > Ji(k)=Ji'
      Ji   = h%J(j,1); Ni = h%N(j,1); Si = h%espin(j,1)
      Jf   = h%J(j,2); Nf = h%N(j,2); Sf = h%espin(j,2)
    ! k-> 
      Ji_p = h%J(k,1); Ni_p = h%N(k,1)
      Jf_p = h%J(k,2); Nf_p = h%N(k,2)
    !
    ! li and lf are the vibrational angular momentum of the vibrational
    ! available for Linear molecules in HITRAN; 
    ! (due to its symmetry though).
    !
      li = h%lv2(1)
      lf = h%lv2(2) 
    ! 
    ! Selecting the initial and final L. Theoretically, is correct to 
    ! define L from 0 to inf (or the larger J in the band); however,
    ! is time comsuming and les efficient 
    ! For further details, see Appendix A of [Niro et al. 2005] 
    !
      iniL=int(max(abs(Ji-Ji_p),abs(Jf-Jf_p)))
      If( mod(iniL,step) .ne. 0 )iniL=iniL+1
      endL=int(min((Ji+Ji_p),(Jf+Jf_p)))
    !
    ! Init. the LLS-P-Matrix to zero. 
    !
      M(1)=0.0_dp;M( 2)=0.0_dp;M( 3)=0.0_dp;M( 4)=0.0_dp
      M(5)=0.0_dp;M( 6)=0.0_dp;M( 7)=0.0_dp;M( 8)=0.0_dp
      M(9)=0.0_dp;M(10)=0.0_dp;M(11)=0.0_dp;M(12)=0.0_dp
    !
      if (molP%M .eq. 7) then
          K1 = Kpart1_O2(Ji, Ji_p, Jf, Jf_p, &
                         Ni, Ni_p, Nf, Nf_p, AF1,econ)
      else
          K1 = Kpart1(Ji_p, Jf, Jf_p, li, lf, AF1 )
      endif
      !
      do L = iniL,endL,step
        !
        dE = abs( real(L*(L+1),dp) )
        coeffC = c_AF(molP, PerM, Ji, real(L,dp), step)
        if (molP%M .eq. 7) then
          K2 = Kpart2_O2(L, Ji, Ji_p, Jf, Jf_p, &
                         Ni, Ni_p, Nf, Nf_p, Si, Sf, AF2, econ)
        else
          K2 = Kpart2( L, Ji, Ji_p, Jf, Jf_p, li, lf, AF2,econ)
        endif
        !
        ! Add to the M(j,:) vector
        M(1) = K2 + M(1)
        M(2) = -K2*log(dE) + M(2)
        M(3) = -K2*dE + M(3)
        M(4) = K2*2*coeffC + M(4)
        M(5) = -K2*2*coeffC*log(dE) + M(5)
        M(6) = -K2*2*coeffC*dE + M(6)
        M(7) = K2*2*coeffC + M(7)
        M(8) = K2*coeffC**2 + M(8)
        M(9) = -K2*log(dE)*coeffC**2 + M(9)
        M(10)= -K2*dE*coeffC**2 + M(10)
        M(11)= K2*coeffC**2 + M(11)
      enddo
      !
      ! DEFINITION OF M(j,:) VECTOR
      ! 
      M(1) = K1 * M(1) 
      M(2) = K1 * M(2)
      M(3) = K1 * (c2*molP%B0/T) * M(3)
      M(4) = K1 * M(4)
      M(5) = K1 * M(5)
      M(6) = K1 * (c2*molP%B0/T) * M(6)
      M(7) = K1 * M(7)
      M(8) = K1 * M(8)
      M(9) = K1 * M(9)
      M(10)= K1 * (c2*molP%B0/T) * M(10)
      M(11)= K1 * M(10)
      M(12)= M(1)

  END SUBROUTINE LLS_AF_Matrix
!--------------------------------------------------------------------------------------------------------------------
