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

#include "itktubeMetaLDA.h"
#include "itktubeMetaNJetLDA.h"
#include "itktubeMetaClassPDF.h"
#include "itktubeMetaRidgeSeed.h"
#include "itktubeMetaTubeParams.h"
#include "itktubePDFSegmenterIO.h"
#include "itktubeRidgeSeedFilterIO.h"

int tubeBaseIOPrintTest( int tubeNotUsed( argc ), char * tubeNotUsed( argv )[] )
{
  itk::tube::MetaLDA metaLDA;
  std::cout << "-------------metaLDA" << std::endl;
  metaLDA.PrintInfo();

  itk::tube::MetaNJetLDA metaNJetLDA;
  std::cout << "-------------metaNJetLDA" << std::endl;
  metaNJetLDA.PrintInfo();

  itk::tube::MetaClassPDF metaClassPDF;
  std::cout << "-------------metaClassPDF" << std::endl;
  metaClassPDF.PrintInfo();

  itk::tube::MetaRidgeSeed metaRidgeSeed;
  std::cout << "-------------metaRidgeSeed" << std::endl;
  metaRidgeSeed.PrintInfo();

  itk::tube::MetaTubeParams metaTubeParams;
  std::cout << "-------------metaTubeParams" << std::endl;
  metaTubeParams.PrintInfo();

  typedef itk::Image< float, 3 > ImageType;
  itk::tube::PDFSegmenterIO< ImageType, 3, ImageType > pdfSegmenterIO;
  std::cout << "-------------pdfSegmenterIO" << std::endl;
  pdfSegmenterIO.PrintInfo();

  itk::tube::RidgeSeedFilterIO< ImageType, ImageType > ridgeSeedFilterIO;
  std::cout << "-------------ridgeSeedFilterIO" << std::endl;
  ridgeSeedFilterIO.PrintInfo();

  return EXIT_SUCCESS;
}
