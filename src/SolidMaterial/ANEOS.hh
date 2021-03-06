//---------------------------------Spheral++----------------------------------//
// ANEOS -- An interface to the ANEOS equation of state from 
// Melosh amony others.  This is a C++ wrapper around calls to the underlying
// ANEOS fortran library.  The user must provide the ancillary file containing
// parameters for ANEOS in its expected format.  Also make sure to keep units
// consistent in this separate input file with what you give the C++ EOS here!
//
// Created by JMO, Tue Apr 23 14:55:28 PDT 2013
//----------------------------------------------------------------------------//
#ifndef ANEOS_HH
#define ANEOS_HH

#include "SolidMaterial/SolidEquationOfState.hh"

#include "boost/multi_array.hpp"

// Forward declarations.
namespace Spheral {

template<typename Dimension, typename DataType> class Field;

template<typename Dimension>
class ANEOS: public SolidEquationOfState<Dimension> {

public:
  //--------------------------- Public Interface ---------------------------//
  typedef typename Dimension::Scalar Scalar;
  typedef typename Dimension::Vector Vector;
  typedef typename Dimension::Tensor Tensor;
  typedef typename Dimension::SymTensor SymTensor;

  typedef typename boost::multi_array<double, 2> array_type;
  typedef typename array_type::array_view<1>::type slice_type;
  typedef typename array_type::const_array_view<1>::type const_slice_type;
  typedef boost::multi_array_types::index_range range;

  // Constructors, destructors.
  ANEOS(const int materialNumber,
        const unsigned numRhoVals,
        const unsigned numTvals,
        const double rhoMin,
        const double rhoMax,
        const double Tmin,
        const double Tmax,
        const PhysicalConstants& constants,
        const double externalPressure,
        const double minimumPressure,
        const double maximumPressure,
        const MaterialPressureMinType minPressureType);
  ~ANEOS();

  // We require any equation of state to define the following properties.
  virtual void setPressure(Field<Dimension, Scalar>& Pressure,
                           const Field<Dimension, Scalar>& massDensity,
                           const Field<Dimension, Scalar>& specificThermalEnergy) const;

  virtual void setTemperature(Field<Dimension, Scalar>& temperature,
                              const Field<Dimension, Scalar>& massDensity,
                              const Field<Dimension, Scalar>& specificThermalEnergy) const;

  virtual void setSpecificThermalEnergy(Field<Dimension, Scalar>& specificThermalEnergy,
                                        const Field<Dimension, Scalar>& massDensity,
                                        const Field<Dimension, Scalar>& temperature) const;

  virtual void setSpecificHeat(Field<Dimension, Scalar>& specificHeat,
                               const Field<Dimension, Scalar>& massDensity,
                               const Field<Dimension, Scalar>& temperature) const;

  virtual void setSoundSpeed(Field<Dimension, Scalar>& soundSpeed,
                             const Field<Dimension, Scalar>& massDensity,
                             const Field<Dimension, Scalar>& specificThermalEnergy) const;

  virtual void setGammaField(Field<Dimension, Scalar>& gamma,
                             const Field<Dimension, Scalar>& massDensity,
                             const Field<Dimension, Scalar>& specificThermalEnergy) const;

  virtual void setBulkModulus(Field<Dimension, Scalar>& bulkModulus,
                             const Field<Dimension, Scalar>& massDensity,
                             const Field<Dimension, Scalar>& specificThermalEnergy) const;

  virtual void setEntropy(Field<Dimension, Scalar>& entropy,
                          const Field<Dimension, Scalar>& massDensity,
                          const Field<Dimension, Scalar>& specificThermalEnergy) const;

  // We also want the equivalent functions for individual calculations.
  Scalar pressure(const Scalar massDensity,
                  const Scalar specificThermalEnergy) const;

  Scalar temperature(const Scalar massDensity,
                     const Scalar specificThermalEnergy) const;

  Scalar specificThermalEnergy(const Scalar massDensity,
                               const Scalar temperature) const;

  Scalar specificHeat(const Scalar massDensity,
                      const Scalar temperature) const;

  Scalar soundSpeed(const Scalar massDensity,
                    const Scalar specificThermalEnergy) const;

  // Get the effective gamma (ratio of specific heats) for this eos.
  Scalar gamma(const Scalar massDensity,
               const Scalar specificThermalEnergy) const;

  // Get the bulk modulus.
  Scalar bulkModulus(const Scalar massDensity,
                     const Scalar specificThermalEnergy) const;

  Scalar entropy(const Scalar massDensity,
                 const Scalar specificThermalEnergy) const;

  // The valid method.
  virtual bool valid() const;

  // Access local variables used to lookup eps based on T.
  int materialNumber() const;
  unsigned numRhoVals() const;
  unsigned numTvals() const;
  double rhoMin() const;
  double rhoMax() const;
  double Tmin() const;
  double Tmax() const;
  const array_type& specificThermalEnergyVals() const;

  // If requested, the user can specify an external pressure to be applied
  // in the pressure calculation.
  double externalPressure() const;
  void externalPressure(const double x);

  double atomicWeight() const;

private:
  //--------------------------- Private Interface ---------------------------//
  // Tables for the temp->energy lookup.
  int mMaterialNumber;
  unsigned mNumRhoVals, mNumTvals;
  double mRhoMin, mRhoMax, mTmin, mTmax, mExternalPressure;
  array_type mSTEvals;

  // ANEOS internal units.
  PhysicalConstants mANEOSunits;

  // Units conversion from ANEOS.
  double mRhoConv, mTconv, mPconv, mEconv, mCVconv, mVelConv, mSconv;

  // Atomic weight.
  double mAtomicWeight;

  // Disallow default constructor
  ANEOS();

  using EquationOfState<Dimension>::mConstants;
};

}

#else

// Forward declaration.
namespace Spheral {
  template<typename Dimension> class ANEOS;
}

#endif
