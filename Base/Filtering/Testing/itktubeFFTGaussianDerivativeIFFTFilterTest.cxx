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

#include "itktubeFFTGaussianDerivativeIFFTFilter.h"

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <vector>
#include <stdlib.h>
#include <sstream>
namespace{

typedef itk::Image<float, 3>              ImageType;
typedef itk::ImageFileWriter< ImageType > WriterType;
typedef itk::tube::FFTGaussianDerivativeIFFTFilter< ImageType, ImageType >
  FunctionType;

typedef itk::ImageRegionIteratorWithIndex<ImageType>  IteratorType;

bool testFilter(int argc, char *argv[]);
ImageType::Pointer generateImage(int sizeImage, int fillMethod);
bool testImageSize(int sizeImage);
bool testImageSizes();
bool setSigmas(int sigma);
bool testSigmas();
bool testDerivatives();
bool testDerivative(FunctionType::Pointer func, int* order, int sigma, int number);
}

int itktubeFFTGaussianDerivativeIFFTFilterTest(int argc, char * argv[])
{
  bool res = true;
  res = testFilter(argc,argv) && res;
  res = testImageSizes() && res;
  res = testSigmas() && res;
  res = testDerivatives() && res;
  return res ? EXIT_SUCCESS : EXIT_FAILURE;
}

namespace{

bool testFilter( int argc, char * argv[] )
{
  if( argc < 7 )
    {
    std::cerr << "Missing arguments." << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " orderX orderY scaleX scaleY inputImage outputImage" <<
                            std::endl;
    return false;
    }

  // Define the dimension of the images
  enum { Dimension = 2 };

  // Define the pixel type
  typedef float PixelType;

  // Declare the types of the images
  typedef itk::Image<PixelType, Dimension>  ImageType;

  // Declare the reader and writer
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType;

  // Declare the type for the Filter
  typedef itk::tube::FFTGaussianDerivativeIFFTFilter< ImageType, ImageType >
    FunctionType;

  // Create the reader and writer
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[5] );
  try
    {
    reader->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during read:\n"  << e;
    return false;
    }

  ImageType::Pointer inputImage = reader->GetOutput();

  FunctionType::Pointer func = FunctionType::New();
  func->SetInput( inputImage );

  FunctionType::OrdersType orders;
  orders[0] = atoi( argv[1] );
  orders[1] = atoi( argv[2] );
  func->SetOrders( orders );
  FunctionType::SigmasType sigmas;
  sigmas[0] = atof( argv[3] );
  sigmas[1] = atof( argv[4] );
  func->SetSigmas( sigmas );
  func->Update();

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( argv[6] );
  writer->SetInput( func->GetOutput() );
  writer->SetUseCompression( true );

  try
    {
    writer->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during write:\n"  << e;
    return false;
    }

  return true;
}

ImageType::Pointer generateImage(int sizeImage, int fillMethod)
{
  ImageType::Pointer image = ImageType::New();
  ImageType::IndexType start;
  start.Fill(0);

  ImageType::SizeType size;
  size.Fill(sizeImage);

  ImageType::RegionType region(start, size);
  region.SetIndex( start );
  region.SetSize( size );

  image->SetRegions(region);
  image->Allocate();
  image->FillBuffer(0);

  IteratorType it( image, image->GetRequestedRegion() );
  while( !it.IsAtEnd() )
    {
    if(fillMethod == 0)
      {
      it.Set( 0.0 );
      }
    else if(fillMethod == 1)
      {
      it.Set( it.GetIndex()[0] == 0 ? 0 : 1 );
      }
    else if(fillMethod == 2)
      {
      it.Set( it.GetIndex()[0] < 5 ? -10 : 10 );
      }
    ++it;
    }
  return image;
}

bool testImageSize(int sizeImage)
{
  ImageType::Pointer image = ImageType::New();
  image = generateImage(sizeImage, 0);

  FunctionType::Pointer func = FunctionType::New();
  func->SetInput( image );

  FunctionType::OrdersType orders;
  orders.Fill(0);
  func->SetOrders( orders );
  FunctionType::SigmasType sigmas;
  sigmas.Fill(1);
  func->SetSigmas( sigmas );

  try
    {
    func->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during filter:\n"  << e;
    return false;
    }
  return true;
}

bool testImageSizes()
{
  int sizeImage = 4;
  bool res = true;
  for(int i = 0; i< sizeImage; i++ )
    {
    res = testImageSize(sizeImage) && res;
    }
  return res;
}

bool setSigmas(int sigma)
{
  int sizeImage = 4;
  ImageType::Pointer image = ImageType::New();
  image = generateImage(sizeImage, 0);

  FunctionType::Pointer func = FunctionType::New();
  func->SetInput( image );

  FunctionType::OrdersType orders;
  orders.Fill(0);
  func->SetOrders( orders );
  FunctionType::SigmasType sigmas;
  sigmas.Fill(sigma);
  func->SetSigmas( sigmas );
  try
    {
    func->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during filter:\n"  << e;
    return false;
    }
  return true;
}

bool testSigmas()
{
  const int maxSigma = 20;
  bool res = true;
  for(int i = 0; i< maxSigma; i++ )
    {
    res = setSigmas(maxSigma) && res;
    }
  return res;
}

bool testDerivatives()
{
  bool res = true;
  int sizeImage = 12;
  ImageType::Pointer image = ImageType::New();
  image = generateImage(sizeImage, 2);

  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( "image0.mha" );
  writer->SetInput( image );

  try
    {
    writer->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during write:\n"  << e;
    return false;
    }
  FunctionType::Pointer func = FunctionType::New();
  func->SetInput( image );

  int orderArray[10][3] =
     { {0, 0, 0}, {0, 0, 1},
     {0, 1, 0}, {1, 0, 0}, {2, 0, 0}, {0, 2, 0},
     {0, 0, 2}, {1, 1, 0}, {1, 0, 1}, {0, 1, 1} };

  for(int i= 0; i<10; i++)
    {
    res = testDerivative(func, orderArray[i], 1, i)&& res;
    }
  image = generateImage(sizeImage, 5);
  WriterType::Pointer writer1 = WriterType::New();
  writer1->SetFileName( "image1.mha" );
  writer1->SetInput( image );
  try
    {
    writer1->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during write:\n"  << e;
    return false;
    }
  func->SetInput( image );
  for(int i= 0; i<10; i++)
    {
    res = testDerivative(func, orderArray[i], 1, 10+ i)&& res;
    }
  return res;
}

bool testDerivative(FunctionType::Pointer func, int* order, int sigma, int number)
{
  FunctionType::OrdersType orders;
  orders[0] = order[0];
  orders[1] = order[1];
  orders[2] = order[2];
  func->SetOrders( orders );
  func->SetOrders( orders );
  FunctionType::SigmasType sigmas;
  sigmas.Fill(sigma);
  func->SetSigmas( sigmas );
  try
    {
    func->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during filter:\n"  << e;
    return false;
    }

 // WriterType::Pointer derivativeWriter = WriterType::New();
  WriterType::Pointer Imagewriter = WriterType::New();
  std::string imageName = "derivative";
  std::string extension = ".mha";
  char* imageNumber;
  sprintf ( imageNumber, "-s%d-%d-%d-%d-num%d", sigma, order[0],
      order[1], order[2], number);
  std::string imageNumberString = std::string(imageNumber);
  imageName = imageName + imageNumberString + extension;
//  derivativeWriter->SetFileName( imageName );
//  derivativeWriter->SetInput( func->GetKernelImage() );

//  try
//    {
//    derivativeWriter->Update();
//    }
//  catch(itk::ExceptionObject& e)
//    {
//    std::cerr << "Exception caught during write:\n"  << e;
//    return false;
//    }
  std::string name = "output";
  name = name + imageNumberString + extension;
  Imagewriter->SetFileName( name );
  Imagewriter->SetInput( func->GetOutput() );

  try
    {
    Imagewriter->Update();
    }
  catch(itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during write:\n"  << e;
    return false;
    }
  return true;
}

} //end namespace
