/* Copyright (C) 2014
 R ichard Larsson <ric.larsson@gmail.com>
 
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

/** Contains the pressure broadening data class
 * \file   pressurebroadeningdata.h
 * 
 * \author Richard Larsson
 * \date   2014-11-06
 **/

#ifndef pressurebroadeningdata_h
#define pressurebroadeningdata_h

#include <cmath>
#include "array.h"
#include "messages.h"
#include "matpackI.h"
#include "complex.h"
#include "jacobian.h"
#include "file.h"



/**
 This class should contain the data and metadata associated with pressure broadening.
 
 The stored quantities are:
 \param mdata  containing the structured data
 \param mdataerror  containing the structured errors of the mdata input
 \param mtype  containing a tag for the type of data/the structure of the data
 */
class PressureBroadeningData
{
public:
    
    enum PB_Type : Index {
        PB_NONE,                          // No pressure broadening
        PB_AIR_BROADENING,                // Air broadening and self broadening only
        PB_AIR_AND_WATER_BROADENING,      // Air, water, and self broadening
        PB_PLANETARY_BROADENING,          // Gas broadening as done for solar system planets
        PB_SD_AIR_VOLUME,                 // HTP in air for SD limit
        PB_HTP_AIR_VOLUME,                // HTP in air
        PB_VOIGT_TEST_WATER,              // Voigt parameters for testing
        PB_SD_TEST_WATER,                 // SD parameters for testing
        PB_PURELY_FOR_TESTING             // Testing tag for new input structures --- can be changed by anyone...
    };
    
    enum class TestParams : Index { sg0=0, sn0, sd0, sm, sA, sg2, sn2, sd2, 
                                    ag0,   an0, ad0, am, aA, ag2, an2, ad2, COUNT };
    
    // Defining an object with no data and no broadening
    PressureBroadeningData() : mtype(PB_NONE), mdata(), mdataerror() {}
    
    // Use these to get the raw data from this class
    const PB_Type& Type() const {return mtype;}
    const ArrayOfVector& Data() const {return mdata;}
    const ArrayOfVector& DataError() const {return mdataerror;}
    
    //  The functions below this point sets up pressure broadening schemes from external catalogs and old ARTSCAT
    
    /** 
     The HITRAN air broadening scheme was used exclusively for a long time by ARTS.
     This function follows that scheme and ARTSCAT-3
     
     The quantities are:
     \param sgam  self broadening gamma
     \param nself  self broadening n
     \param agam  air broadening gamma
     \param nair  air broadening n
     \param air_pressure_DF  air broadening pressure-based frequency shift
     \param d*  variable associated errors.  A tag of -1 is used when the errors are unknown.
     */
    void SetAirBroadeningFromCatalog(const Numeric& sgam, 
                                     const Numeric& nself,
                                     const Numeric& agam,
                                     const Numeric& nair,
                                     const Numeric& air_pressure_DF,
                                     const Numeric& dsgam, 
                                     const Numeric& dnself,
                                     const Numeric& dagam,
                                     const Numeric& dnair,
                                     const Numeric& dair_pressure_DF);
    
    /** 
     The HITRAN air broadening scheme was used exclusively for a long time by ARTS.
     This function follows that scheme and ARTSCAT-3, but also adds water broadening.
     Since it is new, we skip mdataerror usage for now. 
     
     The quantities are:
     \param sgam  self broadening gamma
     \param sn  self broadening n
     \param sdelta  self shift
     \param agam  air broadening gamma
     \param an  air broadening n
     \param adelta  air shift
     \param wgam  water broadening gamma
     \param wn  water broadening n
     \param wdelta  water shift
     */
    void SetAirAndWaterBroadeningFromCatalog(const Numeric& sgam, 
                                             const Numeric& sn, 
                                             const Numeric& sdelta, 
                                             const Numeric& agam,
                                             const Numeric& an,
                                             const Numeric& adelta,
                                             const Numeric& wgam,
                                             const Numeric& wn,
                                             const Numeric& wdelta);
    
    
    /**
     The ESA planetary study introduced species-dependent pressure broadenings.
     This function can read ARTSCAT-4 line data
     
     The quantities are:
     \param sgam  self broadening gamma
     \param nself  self broadening n
     \param foreign_gamma  foreign broadening gamma
     \param n_foreign  foreign broadening n
     \param foreign_pressure_DF  foreign broadening pressure-based frequency shift
     */ 
    void SetPlanetaryBroadeningFromCatalog(const Numeric& sgam, 
                                        const Numeric& nself,
                                        const Vector&  foreign_gamma,
                                        const Vector&  n_foreign,
                                        const Vector&  foreign_pressure_DF);
    
    /**
     TESTING
     
     The quantities are:
     TESTING
     */ 
    void SetSDAirFromCatalog(const Numeric& gamma0,
                             const Numeric& gamma0_exp,
                             const Numeric& gamma2,
                             const Numeric& gamma2_exp,
                             const Numeric& delta0,
                             const Numeric& delta0_exp,
                             const Numeric& delta2,
                             const Numeric& delta2_exp);
    
    /**
     TESTING                                            *
     
     The quantities are:
     TESTING
     */ 
    void SetHTPAirFromCatalog(const Numeric& gamma0,
                              const Numeric& gamma0_exp,
                              const Numeric& gamma2,
                              const Numeric& gamma2_exp,
                              const Numeric& delta0,
                              const Numeric& delta0_exp,
                              const Numeric& delta2,
                              const Numeric& delta2_exp,
                              const Numeric& fvc,
                              const Numeric& fvc_exp,
                              const Numeric& eta,
                              const Numeric& eta_exp);
    
    /**
     TESTING
     
     The quantities are:
     TESTING
     */ 
    void SetTestFromCatalog(const Numeric& gamma0_air,
                            const Numeric& gamma0_air_exp,
                            const Numeric& gamma0_water,
                            const Numeric& gamma0_water_exp,
                            const Numeric& gamma2_air,
                            const Numeric& gamma2_water,
                            const Numeric& delta0_air,
                            const Numeric& delta0_water);
    void GetTestBroadening(Numeric& gamma0,
                           Numeric& gamma2,
                           Numeric& delta0,
                           ConstVectorView vmrs,
                           const Numeric& theta,
                           const Numeric& pressure,
                           const Index h2o_index) const;
    
    /**
      Sets a vector of derivatives that fits with the QuantumIdentifier 
      and partial derivative
      
      
     */
    void SetInternalDerivatives(ComplexVector& derivatives,
                                const ArrayOfRetrievalQuantity& ppd,
                                const QuantumIdentifier& QI,
                                const Numeric& theta,
                                const Numeric& pressure,
                                const Numeric& self_pressure,
                                const Index    this_species,
                                const Index    h2o_species,
                                ConstVectorView vmrs) const;
    
    // Get the Planetary foreign broadening data vector
    ConstVectorView PlanetaryGammaForeign() const { assert(mtype == PB_PLANETARY_BROADENING); return mdata[2]; }
    
    // Get the Planetary foreign broadening exponent data vector
    ConstVectorView PlanetaryNForeign() const { assert(mtype == PB_PLANETARY_BROADENING); return mdata[3]; }
    
    // Get the Planetary foreign shift data vector
    ConstVectorView PlanetaryDeltaForeign() const { assert(mtype == PB_PLANETARY_BROADENING); return mdata[4]; }
    
    // Get the Planetary foreign broadening data from one position
    Numeric PlanetaryGammaForeign(Index ii) const { assert(mtype == PB_PLANETARY_BROADENING); return mdata[2][ii]; }
    
    // Get the Planetary foreign broadening exponent data from one position
    Numeric PlanetaryNForeign(Index ii) const { assert(mtype == PB_PLANETARY_BROADENING); return mdata[3][ii]; }
    
    // Get the Planetary foreign shift data from one position
    Numeric PlanetaryDeltaForeign(Index ii) const {  assert(mtype == PB_PLANETARY_BROADENING); return mdata[4][ii]; }
    
    // Get the Air broadening air pressure broadening
    Numeric AirBroadeningAgam() const { assert(mtype == PB_AIR_BROADENING);  return mdata[2][0]; }
    
    // Get the Air broadening air pressure broadening exponent
    Numeric AirBroadeningNair() const { assert(mtype == PB_AIR_BROADENING);  return mdata[3][0]; }
    
    // Get the Air broadening air pressure shift
    Numeric AirBroadeningPsf()  const { assert(mtype == PB_AIR_BROADENING);  return mdata[4][0]; }
    
    // Get the self pressure broadening
    Numeric Sgam() const { assert(mdata.nelem() != 0); assert(mdata[0].nelem() != 0); return mdata[0][0]; }
    
    // Get the self pressure broadening exponent
    Numeric Nself() const { assert(mdata.nelem() > 0); assert(mdata[1].nelem() != 0); return mdata[1][0]; }
    
    // Get the Air broadening air pressure broadening error
    Numeric AirBroadeningDAgam() const { assert(mtype == PB_AIR_BROADENING);  return mdataerror[2][0]; }
    
    // Get the Air broadening air pressure broadening exponent error
    Numeric AirBroadeningDNair() const { assert(mtype == PB_AIR_BROADENING);  return mdataerror[3][0]; }
    
    // Get the Air broadening air pressure shift error
    Numeric AirBroadeningDPsf()  const { assert(mtype == PB_AIR_BROADENING);  return mdataerror[4][0]; }
    
    // Get the Air broadening self pressure broadening error
    Numeric dSgam()  const { assert(mtype == PB_AIR_BROADENING);  return mdataerror[0][0]; }
    
    // Get the Air broadening self pressure broadening exponent error
    Numeric dNself() const { assert(mtype == PB_AIR_BROADENING);  return mdataerror[1][0]; }
    
     /**
     Method for changing self-broadening if available
     Error if not
     
     The quantities are:
     \param change self broadening gamma
     \param this_species Index for position this species
     \param h2o_species Index for position of h2o species
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeSelf(const Numeric& change, 
                    const Index this_species, 
                    const Index h2o_species, 
                    const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for changing self-broadening exponent if available
     Error if not
     
     The quantities are:
     \param change self broadening gamma
     \param this_species Index for position this species
     \param h2o_species Index for position of h2o species
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeSelfExponent(const Numeric& change, 
                            const Index this_species, 
                            const Index h2o_species, 
                            const ArrayOfIndex& broad_spec_locations);
    
     /**
     Method for setting self-broadening if available
     Error if not
     
     The quantities are:
     \param change self broadening gamma
     \param this_species Index for position this species
     \param h2o_species Index for position of h2o species
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void SetSelf(const Numeric& new_value, 
                 const Index this_species, 
                 const Index h2o_species, 
                 const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for setting self-broadening exponent if available
     Error if not
     
     The quantities are:
     \param change self broadening gamma
     \param this_species Index for position this species
     \param h2o_species Index for position of h2o species
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void SetSelfExponent(const Numeric& new_value, 
                         const Index this_species, 
                         const Index h2o_species, 
                         const ArrayOfIndex& broad_spec_locations);
    
     /**
     Method for changing self-broadening by relative amount if available
     Error if not
     
     The quantities are:
     \param change self broadening gamma
     \param this_species Index for position this species
     \param h2o_species Index for position of h2o species
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeSelfRelative(const Numeric& change, 
                            const Index this_species, 
                            const Index h2o_species, 
                            const ArrayOfIndex& broad_spec_locations);
    
     /**
     Method for changing self-broadening exponent by relative amount if available
     Error if not
     
     The quantities are:
     \param change self broadening gamma
     \param this_species Index for position this species
     \param h2o_species Index for position of h2o species
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeSelfExponentRelative(const Numeric& change, 
                                    const Index this_species, 
                                    const Index h2o_species, 
                                    const ArrayOfIndex& broad_spec_locations);
    
     /**
     Method for changing foreing-broadening(s) if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeForeign(const Numeric& change, 
                       const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for changing foreing-broadening exponent(s) if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeForeignExponent(const Numeric& change, 
                               const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for setting foreing-broadening(s) if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void SetForeign(const Numeric& new_value, 
                    const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for setting foreing-broadening exponent(s) if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void SetForeignExponent(const Numeric& new_value, 
                            const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for changing foreing-broadening(s) by relative amount if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeForeignRelative(const Numeric& change, 
                               const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for changing foreing-broadening exponent(s) by relative amount if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeForeignExponentRelative(const Numeric& change, 
                                       const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for changing foreing-broadening shift(s) by relative amount if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeForeignShiftRelative(const Numeric& change, 
                                    const ArrayOfIndex& broad_spec_locations);
    
    /**
     Method for changing foreing-broadening shift(s) if available
     Error if not available and no change to water broadening
     
     The quantities are:
     \param change self broadening gamma
     \param broad_spec_locations Array of index of broadening species locations
     */ 
    void ChangeForeignShift(const Numeric& change, 
                            const ArrayOfIndex& broad_spec_locations);
    
    // Use these to return data in the format required by the line shape calculator
    
    /**
     Air broadening calculations
     
     The quantities are:
     \param gamma  the pressure broadening in Hz
     \param deltaf  the pressure shift in Hz
     \param theta  the scaled temperature (T0/T)
     \param pressure  All gasses, in Pa
     \param self_pressure  pressure of the molecule the line belongs to
     */
    void GetAirBroadening(Numeric& gamma,
                          Numeric& deltaf,
                          const Numeric& theta,
                          const Numeric& pressure,
                          const Numeric& self_pressure) const;
    void GetAirBroadening_dT(Numeric& dgamma_dT,
                             Numeric& ddeltaf_dT,
                             const Numeric& T,
                             const Numeric& T0,
                             const Numeric& pressure,
                             const Numeric& self_pressure) const;
    void GetAirBroadening_dSelfGamma(Numeric& gamma_dSelf,
                                     const Numeric& theta,
                                     const Numeric& self_pressure) const;
    void GetAirBroadening_dForeignGamma(Numeric& gamma_dForeign,
                                        const Numeric& theta,
                                        const Numeric& pressure,
                                        const Numeric& self_pressure) const;
    void GetAirBroadening_dForeignPsf(Numeric& psf_dForeign,
                                      const Numeric& theta,
                                      const Numeric& pressure) const;
    void GetAirBroadening_dSelfExponent(Numeric& gamma_dSelfExponent,
                                        Numeric& psf_dSelfExponent,
                                        const Numeric& theta,
                                        const Numeric& self_pressure) const;
    void GetAirBroadening_dForeignExponent(Numeric& gamma_dForeignExponent,
                                           Numeric& psf_dForeignExponent,
                                           const Numeric& theta,
                                           const Numeric& pressure,
                                           const Numeric& self_pressure) const;
    void GetPressureBroadeningParams_dSelfVMR(Numeric& gamma_dvmr,
                                              Numeric& split_dvmr,
                                              const Numeric& theta,
                                              const Numeric& pressure) const;

    /**
    Air and water broadening calculations                              *
    
    The quantities are:
    \param gamma  the pressure broadening in Hz
    \param deltaf  the pressure shift in Hz
    \param theta  the scaled temperature (T0/T)
    \param pressure  All gasses, in Pa
    \param self_pressure  pressure of the molecule of the line
    \param verbosity  output command used for warnings.
    */
    void GetAirAndWaterBroadening(Numeric& gamma,
                                  Numeric& deltaf,
                                  const Numeric& theta,
                                  const Numeric& pressure,
                                  const Numeric& self_pressure,
                                  const Index    this_species,
                                  const Index    h2o_species,
                                  ConstVectorView vmrs) const;
    void GetAirAndWaterBroadening_dT(Numeric& dgamma_dT,
                                     Numeric& ddeltaf_dT,
                                     const Numeric& T,
                                     const Numeric& T0,
                                     const Numeric& pressure,
                                     const Numeric& self_pressure,
                                     const Index    this_species,
                                     const Index    h2o_species,
                                     ConstVectorView vmrs) const;
    void GetAirAndWaterBroadening_dSelfGamma(Numeric& dgamma_dSelf,
                                             const Numeric& theta,
                                             const Numeric& self_pressure) const;
    void GetAirAndWaterBroadening_dForeignGamma(Numeric& dgamma_dForeign,
                                                const Numeric& theta,
                                                const Numeric& pressure,
                                                const Numeric& self_pressure,
                                                const Index    this_species,
                                                const Index    h2o_species,
                                                ConstVectorView vmrs) const;
    void GetAirAndWaterBroadening_dWaterGamma(Numeric& gamma_dWater,
                                              const Numeric& theta,
                                              const Numeric& pressure,
                                              const Index    this_species,
                                              const Index    h2o_species,
                                              ConstVectorView vmrs) const;
    void GetAirAndWaterBroadening_dSelfPsf(Numeric& dpsf_dSelf,
                                           const Numeric& theta,
                                           const Numeric& self_pressure) const;
    void GetAirAndWaterBroadening_dForeignPsf(Numeric& dpsf_dForeign,
                                              const Numeric& theta,
                                              const Numeric& pressure,
                                              const Numeric& self_pressure,
                                              const Index    this_species,
                                              const Index    h2o_species,
                                              ConstVectorView vmrs) const;
    void GetAirAndWaterBroadening_dWaterPsf(Numeric& psf_dWater,
                                            const Numeric& theta,
                                            const Numeric& pressure,
                                            const Index    this_species,
                                            const Index    h2o_species,
                                            ConstVectorView vmrs) const;
    void GetAirAndWaterBroadening_dSelfExponent(Numeric& dgamma_dSelfExponent,
                                                Numeric& dpsf_dSelfExponent,
                                                const Numeric& theta,
                                                const Numeric& self_pressure) const;
    void GetAirAndWaterBroadening_dForeignExponent(Numeric& dgamma_dForeignExponent,
                                                   Numeric& dpsf_dForeignExponent,
                                                   const Numeric& theta,
                                                   const Numeric& pressure,
                                                   const Numeric& self_pressure,
                                                   const Index    this_species,
                                                   const Index    h2o_species,
                                                   ConstVectorView vmrs) const;
    void GetAirAndWaterBroadening_dWaterExponent(Numeric& gamma_dWaterExponent,
                                                 Numeric& psf_dWaterExponent,
                                                 const Numeric& theta,
                                                 const Numeric& pressure,
                                                 const Index    this_species,
                                                 const Index    h2o_species,
                                                 ConstVectorView vmrs) const;
    
    /**
     Planetary broadening calculations
     
     The quantities are:
     \param gamma  the pressure broadening in Hz
     \param deltaf  the pressure shift in Hz
     \param theta  the scaled temperature (T0/T)
     \param pressure  All gasses, in Pa
     \param self_pressure  pressure of the molecule the line belongs to
     \param this_species  location of this species in broad_spec_locations and vmrs
     \param broad_spec_locations  location of species that broadens the line in vmrs
     \param vmrs  volume mixing ratio of all species
     \param verbosity  output command used for warnings.
     */
    void GetPlanetaryBroadening(Numeric& gamma,
                             Numeric& deltaf,
                             const Numeric& theta,
                             const Numeric& pressure,
                             const Numeric& self_pressure,
                             const ArrayOfIndex& broad_spec_locations,
                             ConstVectorView vmrs) const;
    void GetPlanetaryBroadening_dT(Numeric& dgamma_dT,
                                Numeric& ddeltaf_dT,
                                const Numeric& T,
                                const Numeric& T0,
                                const Numeric& pressure,
                                const Numeric& self_pressure,
                                const ArrayOfIndex& broad_spec_locations,
                                ConstVectorView vmrs) const;
    
     /**
       All broadening calculations are easiset to perform via these calls.
       If specific derivative is not available, then there will be an error
       
       The quantities are:
      \param gamma  the pressure broadening in Hz
      \param deltaf  the pressure shift in Hz
      \param theta  the scaled temperature (T0/T)
      \param pressure  All gasses, in Pa
      \param self_pressure  pressure of the molecule the line belongs to
      \param this_species  location of this species in broad_spec_locations and vmrs
      \param broad_spec_locations  location of species that broadens the line in vmrs
      \param vmrs  volume mixing ratio of all species
      \param verbosity  output command used for warnings.
      */
     void GetPressureBroadeningParams(Numeric& gamma_0,
                                      Numeric& gamma_2,
                                      Numeric& eta,
                                      Numeric& df_0,
                                      Numeric& df_2,
                                      Numeric& f_VC,
                                      const Numeric& T,
                                      const Numeric& T0,
                                      const Numeric& pressure,
                                      const Numeric& self_pressure,
                                      const Index    this_species,
                                      const Index    h2o_species,
                                      const ArrayOfIndex& broad_spec_locations,
                                      ConstVectorView vmrs) const;
    void GetPressureBroadeningParams_dT(Numeric& dgamma_0_dT,
                                        Numeric& dgamma_2_dT,
                                        Numeric& deta_dT,
                                        Numeric& ddf_0_dT,
                                        Numeric& ddf_2_dT,
                                        Numeric& df_VC_dT,
                                        const Numeric& T,
                                        const Numeric& T0,
                                        const Numeric& pressure,
                                        const Numeric& self_pressure,
                                        const Index    this_species,
                                        const Index    h2o_species,
                                        const ArrayOfIndex& broad_spec_locations,
                                        ConstVectorView vmrs) const;
    void GetPressureBroadeningParams_dSelfGamma(Numeric& gamma_dSelf,
                                                const Numeric& theta,
                                                const Numeric& self_pressure) const;
    void GetPressureBroadeningParams_dForeignGamma(Numeric& gamma_dForeign,
                                                   const Numeric& theta,
                                                   const Numeric& pressure,
                                                   const Numeric& self_pressure,
                                                   const Index    this_species,
                                                   const Index    h2o_species,
                                                   ConstVectorView vmrs) const;
    void GetPressureBroadeningParams_dWaterGamma(Numeric& gamma_dWater,
                                                 const Numeric& theta,
                                                 const Numeric& pressure,
                                                 const Index    this_species,
                                                 const Index    h2o_species,
                                                 ConstVectorView vmrs) const;
    void GetPressureBroadeningParams_dSelfPsf(Numeric& psf_dSelf,
                                              const Numeric& theta,
                                              const Numeric& self_pressure) const;
    void GetPressureBroadeningParams_dForeignPsf(Numeric& psf_dForeign,
                                                 const Numeric& theta,
                                                 const Numeric& pressure,
                                                 const Numeric& self_pressure,
                                                 const Index    this_species,
                                                 const Index    h2o_species,
                                                 ConstVectorView vmrs) const;
    void GetPressureBroadeningParams_dWaterPsf(Numeric& psf_dWater,
                                               const Numeric& theta,
                                               const Numeric& pressure,
                                               const Index    this_species,
                                               const Index    h2o_species,
                                               ConstVectorView vmrs) const;
    void GetPressureBroadeningParams_dSelfExponent(Numeric& gamma_dSelfExponent,
                                                   Numeric& psf_dSelfExponent,
                                                   const Numeric& theta,
                                                   const Numeric& self_pressure) const;
    void GetPressureBroadeningParams_dForeignExponent(Numeric& gamma_dForeignExponent,
                                                      Numeric& psf_dForeignExponent,
                                                      const Numeric& theta,
                                                      const Numeric& pressure,
                                                      const Numeric& self_pressure,
                                                      const Index    this_species,
                                                      const Index    h2o_species,
                                                      ConstVectorView vmrs) const;
    void GetPressureBroadeningParams_dWaterExponent(Numeric& gamma_dWaterExponent,
                                                    Numeric& psf_dWaterExponent,
                                                    const Numeric& theta,
                                                    const Numeric& pressure,
                                                    const Index    this_species,
                                                    const Index    h2o_species,
                                                    ConstVectorView vmrs) const;

    /**
     Speed-dependent broadening calculations
     
     THIS IS EXPERIMENTAL AND MUST BE FIXED TO HAVE, e.g., SELF-PRESSURE
     BEFORE IT IS READY FOR NORMAL USE.
     
     The quantities are:
     \param gamma0 the pressure broadening
     \param gamma2 the speed dependent term of the pressure broadening
     \param delta0 the pressure shift
     \param delta2 the speed dependent term of the pressure shift
     \param theta  the scaled temperature (T0/T)
     \param pressure  All gasses, in Pa
     */
    void GetSDAirBroadening(Numeric& gamma0,
                            Numeric& gamma2,
                            Numeric& delta0,
                            Numeric& delta2,
                            const Numeric& theta,
                            const Numeric& pressure) const;
    void GetSDAirBroadening_dT(Numeric& dgamma0,
                               Numeric& dgamma2,
                               Numeric& ddelta0,
                               Numeric& ddelta2,
                               const Numeric& T,
                               const Numeric& T0,
                               const Numeric& pressure) const;

    /**
     Speed-dependent broadening calculations
     
     THIS IS EXPERIMENTAL AND MUST BE FIXED TO HAVE, e.g., SELF-PRESSURE
     BEFORE IT IS READY FOR NORMAL USE.
     
     The quantities are:
     \param gamma0 the pressure broadening
     \param gamma2 the speed dependent term of the pressure broadening
     \param delta0 the pressure shift
     \param delta2 the speed dependent term of the pressure shift
     \param fvc 
     \param eta 
     \param theta  the scaled temperature (T0/T)
     \param pressure  All gasses, in Pa
     */
    void GetHTPAirBroadening(Numeric& gamma0,
                             Numeric& gamma2,
                             Numeric& delta0,
                             Numeric& delta2,
                             Numeric& fvc,
                             Numeric& eta,
                             const Numeric& theta,
                             const Numeric& pressure) const;
    void GetHTPAirBroadening_dT(Numeric& dgamma0,
                                Numeric& dgamma2,
                                Numeric& ddelta0,
                                Numeric& ddelta2,
                                Numeric& dfvc,
                                Numeric& deta,
                                const Numeric& T,
                                const Numeric& T0,
                                const Numeric& pressure) const;
    
   // Sets the pressure broadening PB_type from String input
   void StorageTag2SetType(const String& input);
   
   // Sets the pressure broadening PB_type from String input
   void SetTypeFromIndex(const Index& type) {mtype = PB_Type(type);};
   
   // Returns length of the vector that is supposed to be input
   Index ExpectedVectorLengthFromType() const;
   
   // Sets the data of the class from vector input
   void SetDataFromVectorWithKnownType(ConstVectorView input);
   
   // Gets the vector from the data of the class
   void GetVectorFromData(Vector& output) const;
   
   // Returns the String tag for this PB_type
   String Type2StorageTag() const;
   
private:
    // mtype identifies the type of of pressure broadening and the other variables
    // are containers
    PB_Type       mtype;
    ArrayOfVector mdata;
    ArrayOfVector mdataerror;
};


class LineFunctionData {
public:
  
  #define SELF "SELF"
  #define BATH "AIR"
  #define NO_LM "#"
  
  // Line shape models
  enum class LineShapeType {DP, LP, VP, SDVP, HTP, };
  
  // Line mixing models
  enum class LineMixingType {None, LM1, LM2, Interp, };
  
  // Temperature dependencies
  enum class TemperatureType {None, T0, T1, T2, T3, T4, LM_AER, };
  
  // Line shape models are defined by these variables (do leave Size at end)
  enum class DopplerParam    : Index {                          Size};
  enum class LorentzParam    : Index {G0, D0,                   Size};
  enum class VoigtParam      : Index {G0, D0,                   Size};
  enum class SpeedVoigtParam : Index {G0, D0, G2, D2,           Size};
  enum class HTPParam        : Index {G0, D0, G2, D2, FVC, ETA, Size};
  
  // Line mixing models are defined by these variables (do leave Size at end)
  enum class NoLineMixingParam : Index {                        Size};
  enum class FirstOrderParam   : Index {Y,                      Size};
  enum class SecondOrderParam  : Index {Y, G, DV,               Size};
  enum class InterpParam       : Index {INTERPOLATED_VARIABLES, Size};
  
  Index LineMixingTypeNelem() const {
    switch(mlm) {
      case LineMixingType::None:
        return Index(NoLineMixingParam::Size);
      case LineMixingType::LM1:
        return Index(FirstOrderParam::Size);
      case LineMixingType::LM2:
        return Index(SecondOrderParam::Size);
      case LineMixingType::Interp:
        return Index(InterpParam::Size);
    }
    return -1;
  }
  
  String LineMixingType2String() const {
    switch(mlm) {
      case LineMixingType::None:
        return NO_LM;
      case LineMixingType::LM1:
        return "LM1";
      case LineMixingType::LM2:
        return "LM2";
      case LineMixingType::Interp:
        return "INT";
    }
    return "-1";
  }
  
  void StringSetLineMixingType(const String& type) {
    if(type == NO_LM)
      mlm = LineMixingType::None;
    else if(type == "LM1")
      mlm = LineMixingType::LM1;
    else if(type == "LM2")
      mlm = LineMixingType::LM2;
    else if(type == "INT")
      mlm = LineMixingType::Interp;
    else {
      ostringstream os;
      os << "Cannot recognize type " << type << " as a line mixing functionality\n" ;
      throw std::runtime_error(os.str());
    }
  }
  
  Index LineShapeTypeNelem() const {
    switch(mp) {
      case LineShapeType::DP:
        return Index(DopplerParam::Size);
      case LineShapeType::LP:
        return Index(LorentzParam::Size);
      case LineShapeType::VP:
        return Index(VoigtParam::Size);
      case LineShapeType::SDVP:
        return Index(SpeedVoigtParam::Size);
      case LineShapeType::HTP:
        return Index(HTPParam::Size);
    }
    return -1;
  }
  
  String LineShapeType2String() const {
    switch(mp) {
      case LineShapeType::DP:
        return "DP";
      case LineShapeType::LP:
        return "LP";
      case LineShapeType::VP:
        return "VP";
      case LineShapeType::SDVP:
        return "SDVP";
      case LineShapeType::HTP:
        return "HTP";
    }
    return "-1";
  }
  
  void StringSetLineShapeType(const String& type) {
    if(type == "DP")
      mp = LineShapeType::DP;
    else if(type == "LP")
      mp = LineShapeType::LP;
    else if(type == "VP")
      mp = LineShapeType::VP;
    else if(type == "SDVP")
      mp = LineShapeType::SDVP;
    else if(type == "HTP")
      mp = LineShapeType::HTP;
    else {
      ostringstream os;
      os << "Cannot recognize type " << type << " as a line shape functionality\n" ;
      throw std::runtime_error(os.str());
    }
  }
  
  Index TemperatureTypeNelem(const TemperatureType type) const {
    switch(type) {
      case TemperatureType::LM_AER:
        return -1;
      case TemperatureType::None:
        return 0;
      case TemperatureType::T0:
        return 1;
      case TemperatureType::T1:
        return 2;
      case TemperatureType::T2:
        return 3;
      case TemperatureType::T3:
        return 2;
      case TemperatureType::T4:
        return 3;
    }
    return -1;
  }
  
  String TemperatureType2String(const TemperatureType type) const {
    switch(type) {
      case TemperatureType::LM_AER:
        return "LM_AER";
      case TemperatureType::None:
        return NO_LM;
      case TemperatureType::T0:
        return "T0";
      case TemperatureType::T1:
        return "T1";
      case TemperatureType::T2:
        return "T2";
      case TemperatureType::T3:
        return "T3";
      case TemperatureType::T4:
        return "T4";
    }
    return "-1";
  }
  
  void StringSetTemperatureType(const Index ispecies, const Index iparam, const String& type) {
    if(type == "LM_AER")
      mtypes[ispecies][iparam] = TemperatureType::LM_AER;
    else if(type == NO_LM)
      mtypes[ispecies][iparam] = TemperatureType::None;
    else if(type == "T0")
      mtypes[ispecies][iparam] = TemperatureType::T0;
    else if(type == "T1")
      mtypes[ispecies][iparam] = TemperatureType::T1;
    else if(type == "T2")
      mtypes[ispecies][iparam] = TemperatureType::T2;
    else if(type == "T3")
      mtypes[ispecies][iparam] = TemperatureType::T3;
    else if(type == "T4")
      mtypes[ispecies][iparam] = TemperatureType::T4;
    else {
      ostringstream os;
      os << "Cannot recognize type " << type << " as a type of temperature type\n" ;
      throw std::runtime_error(os.str());
    }
  }
  
  void GetParams(Numeric& G0, Numeric& D0, 
                 Numeric& G2, Numeric& D2, 
                 Numeric& FVC, Numeric& ETA,
                 Numeric& Y, Numeric& G, Numeric& DV,
                 const Numeric& T0, const Numeric& T,
                 const Numeric& P, const Numeric& self_vmr,
                 const ConstVectorView rtp_vmr, const ArrayOfSpeciesTag& abs_species,
                 const bool normalization=true) const;
  
  // Read data
  const ArrayOfVector& Data() const { return mdata; }
  const ArrayOfSpeciesTag& Species() const { return mspecies; }
  bool SelfBroadening() const { return mself; }
  bool AirBroadening() const { return mbath; }
  LineShapeType LineShape() const { return mp; }
  LineMixingType LineMixing() const { return mlm; }
  
  friend std::ostream& operator<<(std::ostream& os, const LineFunctionData& lfd);
  friend std::istream& operator>>(std::istream& is, LineFunctionData& lfd);
  
private:
  bool mself;
  bool mbath;
  ArrayOfSpeciesTag mspecies;
  Array<Array<TemperatureType>> mtypes;
  ArrayOfVector mdata;
  LineShapeType mp;
  LineMixingType mlm;
};

std::ostream& operator<<(std::ostream& os, const LineFunctionData& lfd);
std::istream& operator>>(std::istream& is, LineFunctionData& lfd);

#endif //pressurebroadeningdata_h
