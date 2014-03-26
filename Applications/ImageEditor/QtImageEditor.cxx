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

//Qt includes
#include <QDebug>
#include <QFileDialog>

//QtImageViewer includes
#include "QtGlSliceView.h"

//ImageEditor includes
#include "QtImageEditor.h"
#include "QtOverlayControlsWidget.h"

namespace tube{

QtImageEditor::QtImageEditor(QWidget* parent, Qt::WindowFlags fl ) :
  QDialog( parent, fl )
{
  this->m_ImageData = 0;
  this->setupUi(this);
  this->Controls->setSliceView(this->OpenGlWindow);

  this->m_TabWidget = new QTabWidget(this);
  m_TabWidget->setMaximumHeight(300);

  m_TabWidget->insertTab(0, this->Controls, "Controls");

  QWidget *filterControlWidget = new QWidget(m_TabWidget);
  m_TabWidget->insertTab(1, filterControlWidget, "Filter");

  QGridLayout *filterGridLayout = new QGridLayout(filterControlWidget);
  this->m_SigmaLineEdit = new QLineEdit();
  QLabel *sigma = new QLabel("Sigma:");
  QHBoxLayout *sigmaHBox = new QHBoxLayout();
  this->m_SigmaLineEdit->setMaximumWidth(80);
  this->m_SigmaLineEdit->setText("0.2");
  sigmaHBox->addWidget(sigma);
  sigmaHBox->addWidget(this->m_SigmaLineEdit);

  QWidget *sigmaWidget = new QWidget();
  sigmaWidget->setLayout(sigmaHBox);
  sigmaWidget->setMaximumWidth(140);

  filterGridLayout->addWidget(sigmaWidget, 0, 0);

  QPushButton *applyButton = new QPushButton();
  applyButton->setText("Apply");
  filterGridLayout->addWidget(applyButton, 0, 1);

  QtOverlayControlsWidget *overlayWidget = new QtOverlayControlsWidget(m_TabWidget);
  overlayWidget->setSliceView(this->OpenGlWindow);
  m_TabWidget->insertTab(2, overlayWidget, "Overlay");

  QDialogButtonBox *buttons = new QDialogButtonBox(Qt::Horizontal);
  //QDialogButtonBox::Close |
  //QDialogButtonBox::Help | QDialogButtonBox::Open,

  QPushButton *loadImageButton = new QPushButton();
  loadImageButton->setText("Load");

  buttons->addButton(loadImageButton, QDialogButtonBox::ActionRole);
  buttons->addButton(this->ButtonHelp, QDialogButtonBox::HelpRole);
  buttons->addButton(this->ButtonOk, QDialogButtonBox::RejectRole);


  this->gridLayout->addWidget(buttons, 4, 0,1,2);
  this->gridLayout->addWidget(m_TabWidget, 2, 0,2,2);

  QObject::connect(buttons, SIGNAL(accepted()), this, SLOT(loadImage()));
  QObject::connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  QObject::connect(buttons, SIGNAL(helpRequested()), OpenGlWindow, SLOT(showHelp()));
  QObject::connect(SliceNumSlider, SIGNAL(sliderMoved(int)), OpenGlWindow,
                   SLOT(changeSlice(int)));
  QObject::connect(OpenGlWindow, SIGNAL(sliceNumChanged(int)), SliceNumSlider,
                   SLOT(setValue(int)));
  QObject::connect(SliceNumSlider, SIGNAL(sliderMoved(int)), this,
                   SLOT(setDisplaySliceNumber(int)));
  QObject::connect(OpenGlWindow, SIGNAL(sliceNumChanged(int)), this,
                   SLOT(setDisplaySliceNumber(int)));
  QObject::connect(applyButton, SIGNAL(clicked()), this, SLOT(setFilter()));
  QObject::connect(this->m_SigmaLineEdit, SIGNAL(textChanged(QString)), this,
                   SLOT(setDisplaySigma(QString)));
  QObject::connect(OpenGlWindow, SIGNAL(orientationChanged(int)), this,
                   SLOT(setMaximumSlice()));

  QObject::connect(loadImageButton, SIGNAL(clicked()), this, SLOT(loadImage()));

  if(!OpenGlWindow->inputImage())
    {
    this->SliceNumSlider->setDisabled(true);
    this->m_TabWidget->setDisabled(true);
    }
}

QtImageEditor::~QtImageEditor()
{
}


void QtImageEditor::setInputImage(ImageType * newImData)
{
  if(!newImData)
    {
    return;
    }
  this->SliceNumSlider->setEnabled(true);
  m_TabWidget->setEnabled(true);
  this->m_ImageData = newImData;
  this->OpenGlWindow->setInputImage(newImData);
  setMaximumSlice();
  this->OpenGlWindow->changeSlice(((this->OpenGlWindow->maxSliceNum() -1)/2));
  this->setDisplaySliceNumber(static_cast<int>
                                (this->OpenGlWindow->sliceNum()));
  this->Controls->setInputImage();
  this->OpenGlWindow->show();
  this->OpenGlWindow->update();
}


void QtImageEditor::setDisplaySliceNumber(int number)
{
  QString tempchar = QString::number(number);
  this->SliceValue->setText(tempchar);
}


int QtImageEditor::loadImage()
{
  ReaderType::Pointer reader = ReaderType::New();
  QString filePathToLoad = QFileDialog::getOpenFileName(
        0,"", QDir::currentPath());

  if(filePathToLoad.isEmpty())
    {
    return 0;
    }
  reader->SetFileName( filePathToLoad.toLatin1().data() );

  qDebug() << "loading image " << filePathToLoad << " ... ";
  try
    {
    reader->Update();
    }
  catch (itk::ExceptionObject & e)
    {
    std::cerr << "Exception in file reader " << std::endl;
    std::cerr << e << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << "Done!" << std::endl;
  setInputImage( reader->GetOutput() );

  show();
}


void QtImageEditor::setDisplaySigma(QString value)
{
  this->m_SigmaLineEdit->setText(value);
}


void QtImageEditor::setFilter()
{
  if(this->m_ImageData == NULL)
    {
    return;
    }
  FilterType::Pointer filterX;
  FilterType::Pointer filterY;

  filterX = FilterType::New();
  filterY = FilterType::New();

  filterX->SetDirection( 0 );   // 0 --> X direction
  filterY->SetDirection( 1 );   // 1 --> Y direction

  filterX->SetOrder( FilterType::ZeroOrder );
  filterY->SetOrder( FilterType::ZeroOrder );

  filterX->SetNormalizeAcrossScale( true );
  filterY->SetNormalizeAcrossScale( true );

  filterX->SetInput( this->m_ImageData );
  filterY->SetInput( filterX->GetOutput() );

  const double sigma = m_SigmaLineEdit->text().toDouble();

  filterX->SetSigma( sigma );
  filterY->SetSigma( sigma );

  filterX->Update();
  filterY->Update();

  this->OpenGlWindow->update();
  setInputImage(filterY->GetOutput());
  show();
}


void QtImageEditor::setMaximumSlice()
{
  this->SliceNumSlider->setMaximum(static_cast<int>
                                   (this->OpenGlWindow->maxSliceNum() -1));
  this->SliceNumSlider->setValue(static_cast<int>
                                 (this->SliceValue->text().toInt()));
}

}
