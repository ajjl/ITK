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

#include "itkLevelSetMotionRegistrationFilter.h"

#include "itkWarpImageFilter.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkVectorCastImageFilter.h"
#include "itkImageFileWriter.h"
#include "itkMath.h"


namespace{
// The following class is used to support callbacks
// on the filter in the pipeline that follows later
template<typename TRegistration>
class ShowProgressObject
{
public:
  ShowProgressObject(TRegistration* o)
    {m_Process = o;}
  void ShowProgress()
    {
    if( ( m_Process->GetElapsedIterations() % 10 ) == 0 )
      {
      std::cout << "Progress: " << m_Process->GetProgress() << "  ";
      std::cout << "Iter: " << m_Process->GetElapsedIterations() << "  ";
      std::cout << "Metric: "   << m_Process->GetMetric()   << "  ";
      std::cout << "RMSChange: " << m_Process->GetRMSChange() << "  ";
      std::cout << std::endl;
      }
    if ( m_Process->GetElapsedIterations() == 750 )
      {
      m_Process->StopRegistration(); }
      }
  typename TRegistration::Pointer m_Process;
};
}

// Template function to fill in an image with a circle.
template <typename TImage>
void
FillWithCircle(
typename TImage::Pointer & image,
double * center,
double radius,
typename TImage::PixelType foregnd,
typename TImage::PixelType backgnd )
{

  typedef itk::ImageRegionIteratorWithIndex<TImage> Iterator;
  Iterator it( image, image->GetBufferedRegion() );

  it.GoToBegin();

  typename TImage::IndexType index;
  double r2 = vnl_math_sqr( radius );

  while( !it.IsAtEnd() )
    {
    index = it.GetIndex();
    double distance = 0;
    for( unsigned int j = 0; j < TImage::ImageDimension; j++ )
      {
      distance += vnl_math_sqr((double) index[j] - center[j]);
      }
    if( distance <= r2 )
      {
      it.Set( foregnd );
      }
    else
      {
      it.Set( backgnd );
      }
    ++it;
    }

  typedef itk::SmoothingRecursiveGaussianImageFilter< TImage, TImage > SmoothingFilterType;
  typename SmoothingFilterType::Pointer smoother = SmoothingFilterType::New();
  smoother->SetInput( image );
  smoother->SetSigma( 1.0 );
  smoother->Update();

  image = smoother->GetOutput();
  image->DisconnectPipeline();
}


// Template function to copy image regions
template <typename TImage>
void
CopyImageBuffer(
TImage *input,
TImage *output )
{
  typedef itk::ImageRegionIteratorWithIndex<TImage> Iterator;
  Iterator outIt( output, output->GetBufferedRegion() );
  for( Iterator inIt( input, output->GetBufferedRegion() ); !inIt.IsAtEnd(); ++inIt, ++outIt )
    {
    outIt.Set( inIt.Get() );
    }

}

int itkLevelSetMotionRegistrationFilterTest(int argc, char * argv [] )
{

  typedef float PixelType;
  enum {ImageDimension = 2};
  typedef itk::Image<PixelType,ImageDimension>  ImageType;
  typedef itk::Vector<float,ImageDimension>     VectorType;
  typedef itk::Image<VectorType,ImageDimension> FieldType;
  typedef ImageType::IndexType                  IndexType;
  typedef ImageType::SizeType                   SizeType;
  typedef ImageType::RegionType                 RegionType;

  //--------------------------------------------------------
  std::cout << "Generate input images and initial deformation field";
  std::cout << std::endl;

  ImageType::SizeValueType sizeArray[ImageDimension] = { 128, 128 };
  SizeType size;
  size.SetSize( sizeArray );

  IndexType index;
  index.Fill( 0 );

  RegionType region;
  region.SetSize( size );
  region.SetIndex( index );

  ImageType::Pointer moving = ImageType::New();
  ImageType::Pointer fixed = ImageType::New();
  FieldType::Pointer initField = FieldType::New();

  moving->SetLargestPossibleRegion( region );
  moving->SetBufferedRegion( region );
  moving->Allocate();

  fixed->SetLargestPossibleRegion( region );
  fixed->SetBufferedRegion( region );
  fixed->Allocate();

  initField->SetLargestPossibleRegion( region );
  initField->SetBufferedRegion( region );
  initField->Allocate();

  double center[ImageDimension];
  double radius;
  PixelType fgnd = 250;
  PixelType bgnd = 15;

  // fill moving with circle
  center[0] = 64; center[1] = 64; radius = 30;
  FillWithCircle<ImageType>( moving, center, radius, fgnd, bgnd );

  // fill fixed with circle
  center[0] = 62; center[1] = 64; radius = 32;
  FillWithCircle<ImageType>( fixed, center, radius, fgnd, bgnd );

  // fill initial deformation with zero vectors
  VectorType zeroVec;
  zeroVec.Fill( 0.0 );
  initField->FillBuffer( zeroVec );

  typedef itk::VectorCastImageFilter<FieldType,FieldType> CasterType;
  CasterType::Pointer caster = CasterType::New();
  caster->SetInput( initField );
  caster->InPlaceOff();

  //-------------------------------------------------------------
  std::cout << "Run registration and warp moving" << std::endl;

  typedef itk::LevelSetMotionRegistrationFilter<ImageType,ImageType,FieldType>
    RegistrationType;
  RegistrationType::Pointer registrator = RegistrationType::New();

  registrator->SetInitialDisplacementField( caster->GetOutput() );
  registrator->SetMovingImage( moving );
  registrator->SetFixedImage( fixed );
  registrator->SetNumberOfIterations( 800 );
  registrator->SetMaximumRMSError( 0.1 );
  registrator->SetStandardDeviations( 1.0 );
  registrator->SetMaximumError( 0.08 );
  registrator->SetMaximumKernelWidth( 10 );
  registrator->SetIntensityDifferenceThreshold( 0.001 );

  // turn on inplace execution
  registrator->InPlaceOn();

  // turn on/off use image spacing
  registrator->UseImageSpacingOn();

  typedef RegistrationType::LevelSetMotionFunctionType FunctionType;

  FunctionType * fptr =
    dynamic_cast<FunctionType *>( registrator->GetDifferenceFunction().GetPointer() );
  if(fptr != ITK_NULLPTR)
    {
    fptr->Print( std::cout );
    }

  // exercise other member variables
  std::cout << "No. Iterations: " << registrator->GetNumberOfIterations() << std::endl;
  std::cout << "Max. kernel error: " << registrator->GetMaximumError() << std::endl;
  std::cout << "Max. kernel width: " << registrator->GetMaximumKernelWidth() << std::endl;

  double v[ImageDimension];
  for ( unsigned int j = 0; j < ImageDimension; j++ )
    {
    v[j] = registrator->GetStandardDeviations()[j];
    }
  registrator->SetStandardDeviations( v );

  typedef ShowProgressObject<RegistrationType> ProgressType;
  ProgressType progressWatch(registrator);

  itk::SimpleMemberCommand<ProgressType>::Pointer command;

  command = itk::SimpleMemberCommand<ProgressType>::New();
  command->SetCallbackFunction(&progressWatch, &ProgressType::ShowProgress);

  registrator->AddObserver( itk::ProgressEvent(), command);

  // warp moving image
  typedef itk::WarpImageFilter<ImageType,ImageType,FieldType> WarperType;
  WarperType::Pointer warper = WarperType::New();

  typedef WarperType::CoordRepType CoordRepType;
  typedef itk::NearestNeighborInterpolateImageFunction<ImageType,CoordRepType>
    InterpolatorType;
  InterpolatorType::Pointer interpolator = InterpolatorType::New();


  warper->SetInput( moving );
  warper->SetDisplacementField( registrator->GetOutput() );
  warper->SetInterpolator( interpolator );
  warper->SetOutputSpacing( fixed->GetSpacing() );
  warper->SetOutputOrigin( fixed->GetOrigin() );
  warper->SetOutputDirection( fixed->GetDirection() );
  warper->SetEdgePaddingValue( bgnd );

  warper->Print( std::cout );

  warper->Update();

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();

  if( argc > 1 )
    {
    writer->SetFileName( argv[1] );
    writer->SetInput( fixed );
    writer->Update();
    }

  if( argc > 2 )
    {
    writer->SetFileName( argv[2] );
    writer->SetInput( moving );
    writer->Update();
    }

  if( argc > 3 )
    {
    writer->SetFileName( argv[3] );
    writer->SetInput( warper->GetOutput() );
    writer->Update();
    }


  // ---------------------------------------------------------
  std::cout << "Compare warped moving and fixed." << std::endl;

  // compare the warp and fixed images
  itk::ImageRegionIterator<ImageType> fixedIter( fixed,
      fixed->GetBufferedRegion() );
  itk::ImageRegionIterator<ImageType> warpedIter( warper->GetOutput(),
      fixed->GetBufferedRegion() );

  unsigned int numPixelsDifferent = 0;
  while( !fixedIter.IsAtEnd() )
    {
    if( itk::Math::NotEqualsComparison(fixedIter.Get() , warpedIter.Get()) )
      {
      numPixelsDifferent++;
      }
    ++fixedIter;
    ++warpedIter;
    }

  std::cout << "Number of pixels different: " << numPixelsDifferent;
  std::cout << std::endl;

  const unsigned int maximumNumberOfPixelsDifferent = 4600;
  if( numPixelsDifferent > maximumNumberOfPixelsDifferent )
    {
    std::cout << "Test failed - too many pixels different." << std::endl;
    std::cout << "Tolerance value is " << maximumNumberOfPixelsDifferent << std::endl;
    return EXIT_FAILURE;
    }

  registrator->Print( std::cout );

  // -----------------------------------------------------------
  std::cout << "Test running registrator without initial deformation field.";
  std::cout << std::endl;

  bool passed = true;
  try
    {
    registrator->SetInput( ITK_NULLPTR );
    registrator->SetNumberOfIterations( 2 );
    registrator->Update();
    }
  catch( itk::ExceptionObject& err )
    {
    std::cout << "Unexpected error." << std::endl;
    std::cout << err << std::endl;
    passed = false;
    }

  if ( !passed )
    {
    std::cout << "Test failed" << std::endl;
    return EXIT_FAILURE;
    }

  //--------------------------------------------------------------
  std::cout << "Test exception handling." << std::endl;

  std::cout << "Test ITK_NULLPTR moving image. " << std::endl;
  passed = false;
  try
    {
    registrator->SetInput( caster->GetOutput() );
    registrator->SetMovingImage( ITK_NULLPTR );
    registrator->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cout << "Caught expected error." << std::endl;
    std::cout << err << std::endl;
    passed = true;
    }

  if ( !passed )
    {
    std::cout << "Test failed" << std::endl;
    return EXIT_FAILURE;
    }
  registrator->SetMovingImage( moving );
  registrator->ResetPipeline();

  std::cout << "Test ITK_NULLPTR moving image interpolator. " << std::endl;
  passed = false;
  try
    {
    fptr = dynamic_cast<FunctionType *>(registrator->GetDifferenceFunction().GetPointer() );
    if(fptr == ITK_NULLPTR)
      {
      std::cout << "Test failed - too many pixels different." << std::endl;
      return EXIT_FAILURE;
      }
    fptr->SetMovingImageInterpolator( ITK_NULLPTR );
    registrator->SetInput( initField );
    registrator->Update();
    }
  catch( itk::ExceptionObject & err )
    {
    std::cout << "Caught expected error." << std::endl;
    std::cout << err << std::endl;
    passed = true;
    }

  if ( !passed )
    {
    std::cout << "Test failed" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Test passed" << std::endl;
  return EXIT_SUCCESS;


}
