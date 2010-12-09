/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
#ifndef _itkAnisotropicDiffusiveRegistrationFunction_txx
#define _itkAnisotropicDiffusiveRegistrationFunction_txx

#include "itkAnisotropicDiffusiveRegistrationFunction.h"
#include "itkVectorIndexSelectionCastImageFilter.h"

namespace itk
{

/**
 * Constructor
 */
template < class TFixedImage, class TMovingImage, class TDeformationField >
AnisotropicDiffusiveRegistrationFunction
 < TFixedImage, TMovingImage, TDeformationField >
::AnisotropicDiffusiveRegistrationFunction()
{
  typename Superclass::RadiusType r;
  r.Fill(1);
  this->SetRadius(r);

  m_ComputeRegularizationTerm = true;
  m_ComputeIntensityDistanceTerm = true;
  m_UseAnisotropicRegularization = true;

  m_RegularizationFunction = RegularizationFunctionType::New();
  this->SetTimeStep( 1.0 );
  m_IntensityDistanceFunction = IntensityDistanceFunctionType::New();

  this->SetMovingImage(0);
  this->SetFixedImage(0);
}

/**
 * PrintSelf
 */
template < class TFixedImage, class TMovingImage, class TDeformationField >
void
AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::PrintSelf( std::ostream& os, Indent indent ) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "TimeStep: " << m_TimeStep;
  os << indent << "ComputeRegularizationTerm: "
      << m_ComputeRegularizationTerm << std::endl;
  os << indent << "ComputeIntensityDistanceTerm: "
      << m_ComputeIntensityDistanceTerm << std::endl;
  os << indent << "UseAnisotropicRegularization: "
      << m_UseAnisotropicRegularization << std::endl;
  if ( m_RegularizationFunction )
    {
    os << indent << "RegularizationFunction: " << std::endl;
    m_RegularizationFunction->Print( os, indent );
    }
  if ( m_IntensityDistanceFunction )
    {
    os << indent << "IntensityDistanceFunction: " << std::endl;
    m_IntensityDistanceFunction->Print( os, indent );
    }
}

/**
 * Creates a pointer to the data structure used to manage global values
 */
template < class TFixedImage, class TMovingImage, class TDeformationField >
void *
AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::GetGlobalDataPointer() const
{
  GlobalDataStruct * ans = new GlobalDataStruct();

  // Create the component global data pointers
  if( m_ComputeRegularizationTerm )
    {
    ans->m_RegularizationGlobalDataStruct =
        m_RegularizationFunction->GetGlobalDataPointer();
    }
  if( m_ComputeIntensityDistanceTerm )
    {
    ans->m_IntensityDistanceGlobalDataStruct =
        m_IntensityDistanceFunction->GetGlobalDataPointer();
    }

  return ans;
}

/**
  * Deletes the global data structure
  */
template < class TFixedImage, class TMovingImage, class TDeformationField >
void
AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::ReleaseGlobalDataPointer(void *GlobalData) const
{
  GlobalDataStruct * gd = ( GlobalDataStruct * ) GlobalData;

  // Release the component data structures
  if( m_ComputeRegularizationTerm )
    {
    m_RegularizationFunction->ReleaseGlobalDataPointer(
        gd->m_RegularizationGlobalDataStruct );
    }
  if( m_ComputeIntensityDistanceTerm )
    {
    m_IntensityDistanceFunction->ReleaseGlobalDataPointer(
        gd->m_IntensityDistanceGlobalDataStruct );
    }

  delete gd;
}

/**
  * Called at the beginning of each iteration
  */
template < class TFixedImage, class TMovingImage, class TDeformationField >
void
AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::InitializeIteration()
{
  std::cout << "\tInitializeIteration for FUNCTION" << std::endl;

  if( !this->GetMovingImage() || !this->GetFixedImage()
    || !this->GetDeformationField() )
    {
    itkExceptionMacro( << "MovingImage, FixedImage and/or deformation field "
                       << "not set" );
    }

  // Setup and initialize the component functions
  if( m_ComputeIntensityDistanceTerm )
    {
    m_IntensityDistanceFunction->SetMovingImage( this->GetMovingImage() ) ;
    m_IntensityDistanceFunction->SetFixedImage( this->GetFixedImage() );
    m_IntensityDistanceFunction->SetDeformationField(
        this->GetDeformationField() );
    m_IntensityDistanceFunction->InitializeIteration();
    }
  if( m_ComputeRegularizationTerm )
    {
    m_RegularizationFunction->InitializeIteration();
    }
}

/**
  * Computes the update term
  */
template < class TFixedImage, class TMovingImage, class TDeformationField >
typename AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::PixelType
AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::ComputeUpdate(const NeighborhoodType &, void *, const FloatOffsetType & )
{
  // This function should never be called!
  itkExceptionMacro( << "ComputeUpdate(neighborhood, gd, offset) should never"
                     << "be called.  Use ComputeUpdate(neighborhood, "
                     << "normalVectorImageNeighborhood,"
                     << "tangentialNeighborhoodTensor,"
                     << "tangentialNeighborhoodDeformationFieldComponents,"
                     << "normalNeighborhoodTensor,"
                     << "normalNeighborhoodDeformationFieldComponents,"
                     << " globalData, offset) instead" );
}

/**
  * Computes the update term
  */
template < class TFixedImage, class TMovingImage, class TDeformationField >
typename AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::PixelType
AnisotropicDiffusiveRegistrationFunction
  < TFixedImage, TMovingImage, TDeformationField >
::ComputeUpdate(
    const NeighborhoodType & neighborhood,
    const NormalVectorImageNeighborhoodIteratorType &
        normalVectorImageNeighborhood,
    const DiffusionTensorNeighborhoodIteratorType &
        tangentialNeighborhoodTensor,
    const DeformationVectorComponentNeighborhoodIteratorArrayType &
        tangentialNeighborhoodDeformationFieldComponents,
    const DiffusionTensorNeighborhoodIteratorType &
        normalNeighborhoodTensor,
    const DeformationVectorComponentNeighborhoodIteratorArrayType &
        normalNeighborhoodDeformationFieldComponents,
    void * gd,
    const FloatOffsetType & offset)
{
  // Get the global data structure
  GlobalDataStruct * globalData = ( GlobalDataStruct * ) gd;

  // Get the normal at this pixel
  const typename FixedImageType::IndexType index = neighborhood.GetIndex();

  // Iterate over the deformation field components to compute the regularization
  // and intensity distance terms
  PixelType                         tangentialRegularizationTerm;
  PixelType                         normalRegularizationTerm;
  PixelType                         intensityDistanceTerm;
  PixelType                         updateTerm;

  intensityDistanceTerm.Fill(0);
  tangentialRegularizationTerm.Fill(0);
  normalRegularizationTerm.Fill(0); // essential because incremented in loop

  // Compute the intensity distance update
  if ( m_ComputeIntensityDistanceTerm )
    {
    intensityDistanceTerm = m_IntensityDistanceFunction->ComputeUpdate(
        neighborhood, globalData->m_IntensityDistanceGlobalDataStruct, offset );
    }

  // Compute the motion field regularization
  if (m_ComputeRegularizationTerm )
    {
    NormalVectorType                  normalVector;
    DeformationVectorComponentType    intermediateNormalRegularizationComponent;
    PixelType                         intermediateNormalRegularizationTerm;
    NormalVectorType                  nln; // n(l)n

    if( m_UseAnisotropicRegularization )
      {
      normalVector
          = normalVectorImageNeighborhood.GetImagePointer()->GetPixel( index );
      }

    for ( unsigned int i = 0; i < ImageDimension; i++ )
      {
      // Compute the regularization in the tangential plane
      tangentialRegularizationTerm[i]
          = m_RegularizationFunction->ComputeUpdate(
              tangentialNeighborhoodDeformationFieldComponents[i],
              tangentialNeighborhoodTensor,
              globalData->m_RegularizationGlobalDataStruct,
              offset );

      if( m_UseAnisotropicRegularization )
        {
        // Compute the regularization in the normal direction
        intermediateNormalRegularizationComponent
            = m_RegularizationFunction->ComputeUpdate(
                normalNeighborhoodDeformationFieldComponents[i],
                normalNeighborhoodTensor,
                globalData->m_RegularizationGlobalDataStruct,
                offset );

        nln = normalVector[i] * normalVector;

        intermediateNormalRegularizationTerm
            = intermediateNormalRegularizationComponent * nln;
        normalRegularizationTerm
            = normalRegularizationTerm + intermediateNormalRegularizationTerm;
        }
      }

    }

  updateTerm = intensityDistanceTerm
               + tangentialRegularizationTerm + normalRegularizationTerm;

  return updateTerm;

}

} // end namespace itk

#endif