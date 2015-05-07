/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#ifndef itkMath_h
#define itkMath_h

#include "itkIntTypes.h"
#include "itkMathDetail.h"
#include "itkConceptChecking.h"
#include "itkNumericTraits.h"

namespace itk
{
namespace Math
{
// These constants originate from VXL's vnl_math.h. They have been
// moved here to improve visibility, and to ensure that the constants
// are available during compile time ( as opposed to static const
// member vaiables ).

/** \brief \f[e\f] The base of the natural logarithm or Euler's number */
static const double e                = 2.7182818284590452354;
/** \brief  \f[ \log_2 e \f] */
static const double log2e            = 1.4426950408889634074;
/** \brief \f[ \log_{10} e \f] */
static const double log10e           = 0.43429448190325182765;
/** \brief \f[ \log_e 2 \f] */
static const double ln2              = 0.69314718055994530942;
/** \brief \f[ \log_e 10 \f] */
static const double ln10             = 2.30258509299404568402;
/** \brief \f[ \pi \f]  */
static const double pi               = 3.14159265358979323846;
/** \brief \f[ \frac{\pi}{2} \f]  */
static const double pi_over_2        = 1.57079632679489661923;
/** \brief \f[ \frac{\pi}{4} \f]  */
static const double pi_over_4        = 0.78539816339744830962;
/** \brief \f[ \frac{1}{\pi} \f]  */
static const double one_over_pi      = 0.31830988618379067154;
/** \brief \f[ \frac{2}{\pi} \f]  */
static const double two_over_pi      = 0.63661977236758134308;
/** \brief \f[ \frac{2}{\sqrt{\pi}} \f]  */
static const double two_over_sqrtpi  = 1.12837916709551257390;
/** \brief \f[ \frac{2}{\sqrt{2\pi}} \f]  */
static const double one_over_sqrt2pi = 0.39894228040143267794;
/** \brief \f[ \sqrt{2} \f]  */
static const double sqrt2            = 1.41421356237309504880;
/** \brief \f[ \sqrt{ \frac{1}{2}} \f] */
static const double sqrt1_2          = 0.70710678118654752440;

/** A useful macro to generate a template floating point to integer
 *  conversion templated on the return type and using either the 32
 *  bit, the 64 bit or the vanilla version */
#define itkTemplateFloatingToIntegerMacro(name)                                     \
  template< typename TReturn, typename TInput >                                     \
  inline TReturn name(TInput x)                                                     \
    {                                                                               \
                                                                                    \
    if ( sizeof( TReturn ) <= 4 )                                                   \
      {                                                                             \
      return static_cast< TReturn >( Detail::name##_32(x) );                      \
      }                                                                             \
    else if ( sizeof( TReturn ) <= 8 )                                              \
      {                                                                             \
      return static_cast< TReturn >( Detail::name##_64(x) );                      \
      }                                                                             \
    else                                                                            \
      {                                                                             \
      return static_cast< TReturn >( Detail::name##_base< TReturn, TInput >(x) ); \
      }                                                                             \
    }

/** \brief Round towards nearest integer
 *
 *  \tparam TReturn must be an integer type
 *  \tparam TInput must be float or double
 *
 *          halfway cases are rounded towards the nearest even
 *          integer, e.g.
 *  \code
 *          RoundHalfIntegerToEven( 1.5) ==  2
 *          RoundHalfIntegerToEven(-1.5) == -2
 *          RoundHalfIntegerToEven( 2.5) ==  2
 *          RoundHalfIntegerToEven( 3.5) ==  4
 *  \endcode
 *
 *  The behavior of overflow is undefined due to numerous implementations.
 *
 *  \warning We assume that the rounding mode is not changed from the default
 *  one (or at least that it is always restored to the default one).
 */
itkTemplateFloatingToIntegerMacro(RoundHalfIntegerToEven);

/** \brief Round towards nearest integer
 *
 *  \tparam TReturn must be an integer type
 *  \tparam TInput must be float or double
 *
 *          halfway cases are rounded upward, e.g.
 *  \code
 *          RoundHalfIntegerUp( 1.5) ==  2
 *          RoundHalfIntegerUp(-1.5) == -1
 *          RoundHalfIntegerUp( 2.5) ==  3
 *  \endcode
 *
 *  The behavior of overflow is undefined due to numerous implementations.
 *
 *  \warning The argument absolute value must be less than
 *  NumbericTraits<TReturn>::max()/2 for RoundHalfIntegerUp to be
 *  guaranteed to work.
 *
 *  \warning We also assume that the rounding mode is not changed from
 *  the default one (or at least that it is always restored to the
 *  default one).
 */
itkTemplateFloatingToIntegerMacro(RoundHalfIntegerUp);

/** \brief Round towards nearest integer (This is a synonym for RoundHalfIntegerUp)
 *
 *  \tparam TReturn must be an integer type
 *  \tparam TInput must be float or double
 *
 *  \sa RoundHalfIntegerUp<TReturn, TInput>()
 */
template< typename TReturn, typename TInput >
inline TReturn Round(TInput x) { return RoundHalfIntegerUp< TReturn, TInput >(x); }

/** \brief Round towards minus infinity
 *
 *  The behavior of overflow is undefined due to numerous implementations.
 *
 *  \warning argument absolute value must be less than
 *  NumbericTraits<TReturn>::max()/2 for vnl_math_floor to be
 *  guaranteed to work.
 *
 *  \warning We also assume that the rounding mode is not changed from
 *  the default one (or at least that it is always restored to the
 *  default one).
 */
itkTemplateFloatingToIntegerMacro(Floor);

/** \brief Round towards plus infinity
 *
 *  The behavior of overflow is undefined due to numerous implementations.
 *
 *  \warning argument absolute value must be less than INT_MAX/2
 *  for vnl_math_ceil to be guaranteed to work.
 *  \warning We also assume that the rounding mode is not changed from
 *  the default one (or at least that it is always restored to the
 *  default one).
 */
itkTemplateFloatingToIntegerMacro(Ceil);

#undef  itkTemplateFloatingToIntegerMacro

template< typename TReturn, typename TInput >
inline TReturn CastWithRangeCheck(TInput x)
{
#ifdef ITK_USE_CONCEPT_CHECKING
  itkConceptMacro( OnlyDefinedForIntegerTypes1, ( itk::Concept::IsInteger< TReturn > ) );
  itkConceptMacro( OnlyDefinedForIntegerTypes2, ( itk::Concept::IsInteger< TInput > ) );
#endif // ITK_USE_CONCEPT_CHECKING

  TReturn ret = static_cast< TReturn >( x );
  if ( sizeof( TReturn ) > sizeof( TInput )
       && !( !itk::NumericTraits< TReturn >::is_signed &&  itk::NumericTraits< TInput >::is_signed ) )
    {
    // if the output type is bigger and we are not converting a signed
    // integer to an unsigned integer then we have no problems
    return ret;
    }
  else if ( sizeof( TReturn ) >= sizeof( TInput ) )
    {
    if ( itk::NumericTraits< TInput >::IsPositive(x) != itk::NumericTraits< TReturn >::IsPositive(ret) )
      {
      itk::RangeError _e(__FILE__, __LINE__);
      throw _e;
      }
    }
  else if ( static_cast< TInput >( ret ) != x
            || ( itk::NumericTraits< TInput >::IsPositive(x) != itk::NumericTraits< TReturn >::IsPositive(ret) ) )
    {
    itk::RangeError _e(__FILE__, __LINE__);
    throw _e;
    }
  return ret;
}

/** \brief Return the signed distance in ULPs (units in the last place) between two floats.
 *
 * This is the signed distance, i.e., if x1 > x2, then the result is positive.
 *
 * \sa FloatAlmostEqual
 */
template <typename T>
inline typename Detail::FloatIEEE<T>::IntType
FloatDifferenceULP( T x1, T x2 )
{
  Detail::FloatIEEE<T> x1f(x1);
  Detail::FloatIEEE<T> x2f(x2);
  return x1f.AsULP() - x2f.AsULP();
}

/** \brief Compare two floats and return if they are effectively equal.
 *
 * Determining when floats are almost equal is difficult because of their
 * IEEE bit representation.  This function uses the integer representation of
 * the float to determine if they are almost equal.
 *
 * The implementation is based off the explanation in the white papers:
 *
 * - http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
 * - http://www.cygnus-software.com/papers/comparingfloats/comparingfloats.htm
 *
 * This function is not a cure-all, and reading those articles is important
 * to understand its appropriate use in the context of ULPs, zeros, subnormals,
 * infinities, and NANs.  For example, it is preferable to use this function on
 * two floats directly instead of subtracting them and comparing them to zero.
 *
 * The tolerance is specified in ULPs (units in the last place), i.e. how many
 * floats there are in between the numbers.  Therefore, the tolerance depends on
 * the magnitude of the values that are being compared.  A second tolerance is
 * a maximum difference allowed, which is important when comparing numbers close to
 * zero.
 *
 * A NAN compares as not equal to a number, but two NAN's may compare as equal
 * to each other.
 *
 * \param x1                    first floating value to compare
 * \param x2                    second floating values to compare
 * \param maxUlps               maximum units in the last place to be considered equal
 * \param maxAbsoluteDifference maximum absolute difference to be considered equal
 */
template <typename T>
inline bool
FloatAlmostEqual( T x1, T x2,
  typename Detail::FloatIEEE<T>::IntType maxUlps = 4,
  typename Detail::FloatIEEE<T>::FloatType maxAbsoluteDifference = 0.1*NumericTraits<T>::epsilon() )
{
  // Check if the numbers are really close -- needed
  // when comparing numbers near zero.
  const T absDifference = std::abs(x1 - x2);
  if ( absDifference <= maxAbsoluteDifference )
    {
    return true;
    }

  typename Detail::FloatIEEE<T>::IntType
    ulps = FloatDifferenceULP(x1, x2);
  if(ulps < 0)
    {
    ulps = -ulps;
    }
  return ulps <= maxUlps;
}


/** \brief Provide consistent equality checks between values of potentially different types
 *
 * template< typename T1, typename T2 >
 * EqualsComparison( T1 x1, T2 x2 )
 *
 * template< typename T1, typename T2 >
 * NotEqualsComparison( T1 x1, T2 x2 )
 *
 * This function uses the FloatAlmostEqual() to compare floating point values with
 * values of different types. For maximum extensibility the function is implemented through
 * a series of templated structs which direct the EqualsComparison() call to the correct function
 * by evaluating the parameter types.
 *
 * Overall algorithm:
 *   To compare two floating point types...
 *     use FloatAlmostEqual.
 *
 *   To compare a floating point and an integer type...
 *     If the integer type value is 0 or 1 ...
 *        use NumericTraits<FloatingPointType>::ZeroValue() or ::OneValue() and call
 *        FloatAlmostEqual
 *     Else
 *        Use static_cast<FloatingPointType>(integerValue) and call FloatAlmostEqual
 *
 *   To compare signed and unsigned integers...
 *     Check for negative value or overflow, then cast and use ==
 *
 *   To compare two sogned or two unsigned integers ...
 *     Use ==
 *
 *   To compare anything else ...
 *     Use ==
 *
 */

//Cases for choosing EqualsComparison function
//default case
template<int CASE_NUMBER>
struct CASE
{
  template <typename INPUTtype_1, typename INPUTtype_2>
  static bool func(INPUTtype_1 x1, INPUTtype_2 x2)
  {
    return x1 == x2;
  }
};

//case 1 is that of floating type v. floating type
template<>
struct CASE <1>
{
  template <typename FLOATtype_1, typename FLOATtype_2>
  static bool func(FLOATtype_1 x1, FLOATtype_2 x2)
  {
    return FloatAlmostEqual<double>(x1, x2);
  }

  template <typename FLOATtype_1, typename FLOATtype_2>
  static bool
  func(double x1, double x2)
  {
    return FloatAlmostEqual<double>(x1, x2);
  }

  template <typename FLOATtype_1, typename FLOATtype_2>
  static bool
  func(double x1, float x2)
  {
    return FloatAlmostEqual<double>(x1, x2);
  }

  template <typename FLOATtype_1, typename FLOATtype_2>
  static bool
  func(float x1, double x2)
  {
    return FloatAlmostEqual<double>(x1, x2);
  }

  template <typename FLOATtype_1, typename FLOATtype_2>
  static bool
  func(float x1, float x2)
  {
    return FloatAlmostEqual<float>(x1, x2);
  }
};

//case 2 is floating type v. int type
template<>
struct CASE <2>
{
  template <typename FLOAT_TYPE, typename INT_TYPE>
  static bool func(FLOAT_TYPE floatingVar, INT_TYPE integerVar)
  {
  switch( integerVar )
  {
  case 0:
    return FloatAlmostEqual<FLOAT_TYPE>( floatingVar, itk::NumericTraits<FLOAT_TYPE>::ZeroValue() );
  case 1:
    return FloatAlmostEqual<FLOAT_TYPE>( floatingVar, itk::NumericTraits<FLOAT_TYPE>::OneValue() );
  default:
    return FloatAlmostEqual<FLOAT_TYPE> (floatingVar, integerVar);
  }
  }
};

//case 3 is int type v. floating type
template<>
struct CASE <3>
{
  template <typename INT_TYPE, typename FLOAT_TYPE>
  static bool func(INT_TYPE integerVar, FLOAT_TYPE floatingVar)
  {
    return CASE<2>::func(floatingVar, integerVar);
  }
};

//case 4 is signed type v. unsigned type
template<>
struct CASE <4>
{
  template <typename SIGNED_INT, typename UNSIGNED_INT>
  static bool func(SIGNED_INT signed_x1, UNSIGNED_INT unsigned_x2)
  {
    if(signed_x1 < 0) return false;
    if( unsigned_x2 > static_cast< size_t >(itk::NumericTraits<SIGNED_INT>::max()) ) return false;
    return signed_x1 == static_cast< SIGNED_INT >(unsigned_x2) ;
  }
};

//case 5 is unsigned type v. signed type
template<>
struct CASE <5>
{
  template <typename UNSIGNED_INT, typename SIGNED_INT>
  static bool func(UNSIGNED_INT unsigned_x1, SIGNED_INT signed_x2)
  {
    return CASE<4>::func(signed_x2, unsigned_x1);
  }
};

//case 6 is that of int v. int both signed or both unsigned
template<>
struct CASE <6>
{
  template <typename INTtype_1, typename INTtype_2>
  static bool func(INTtype_1 x1, INTtype_2 x2)
  {
    return x1 == x2;
  }
};
/// end of all the cases

//SELECTOR STRUCTS, these select the correct /case based on its types
//       input1 is int?  input 1 is signed? input2 is int?  input 2 is signed?
template<bool INP_1_INT, bool INP_1_SIGNED, bool INP_2_INT, bool INP_2_SIGNED>
struct SELECTOR
{ //default case
  typedef CASE <0> SELECTED;
};

template<>
struct SELECTOR < false, true, false, true>
//floating type v floating type
{
  typedef CASE <1> SELECTED;
};

template<>
struct SELECTOR <false, true, true, true>
//float vs signed int
{
  typedef CASE <2> SELECTED;
};

template<>
struct SELECTOR <false, true, true,false>
//float vs unsigned int
{
  typedef CASE <2> SELECTED;
};

template<>
struct SELECTOR <true, false, false, true>
//unsigned int vs float
{
  typedef CASE <3> SELECTED;
};

template<>
struct SELECTOR <true, true, false, true>
//signed int vs float
{
  typedef CASE <3> SELECTED;
};

template<>
struct SELECTOR<true, true, true, false>
//signed vs unsigned
{
  typedef CASE<4> SELECTED;
};

template<>
struct SELECTOR<true, false, true, true>
//unsigned vs signed
{
  typedef CASE<5> SELECTED;
};

template<>
struct SELECTOR<true, true, true, true>
//signed vs signed
{
  typedef CASE<6> SELECTED;
};

template<>
struct SELECTOR<true, false, true, false>
//unsigned vs unsigned
{
  typedef CASE<6> SELECTED;
};
//////////end of SELECTOR structs

//The implementor tells the selector what to do
template<typename U1, typename U2>
struct IMPLEMENTOR
{
  typedef typename itk::NumericTraits<U1>::IsInteger U1_isInt;
  typedef typename itk::NumericTraits<U1>::IsSigned U1_isSigned;
  typedef typename itk::NumericTraits<U2>::IsInteger U2_isInt;
  typedef typename itk::NumericTraits<U2>::IsSigned U2_isSigned;

  static const bool U1_Ival = U1_isInt::Value;
  static const bool U1_Sval = U1_isSigned::Value;
  static const bool U2_Ival = U2_isInt::Value;
  static const bool U2_Sval = U2_isSigned::Value;

  typedef typename SELECTOR< U1_Ival, U1_Sval, U2_Ival, U2_Sval>::SELECTED FUNCTION;
};

//The EqualsComparison function
template <typename T1, typename T2>
inline bool
EqualsComparison( T1 x1, T2 x2 )
{
  return IMPLEMENTOR<T1,T2>::FUNCTION::template func<T1, T2>(x1, x2);
}

//The NotEqualsComparison function
template <typename T1, typename T2>
inline bool
NotEqualsComparison( T1 x1, T2 x2 )
{
  return ! EqualsComparison( x1, x2 );
}
//End of structs and functions used for EqualsComparison(), and NotEqualsComparison().


/** Return whether the number in a prime number or not.
 *
 * \note Negative numbers can not be prime.
 */
ITKCommon_EXPORT bool IsPrime( unsigned short n );
ITKCommon_EXPORT bool IsPrime( unsigned int n );
ITKCommon_EXPORT bool IsPrime( unsigned long n );
ITKCommon_EXPORT bool IsPrime( unsigned long long n );


/** Return the greatest factor of the decomposition in prime numbers */
ITKCommon_EXPORT unsigned short     GreatestPrimeFactor( unsigned short n );
ITKCommon_EXPORT unsigned int       GreatestPrimeFactor( unsigned int n );
ITKCommon_EXPORT unsigned long      GreatestPrimeFactor( unsigned long n );
ITKCommon_EXPORT unsigned long long GreatestPrimeFactor( unsigned long long n );

} // end namespace Math
} // end namespace itk
#endif // end of itkMath.h
