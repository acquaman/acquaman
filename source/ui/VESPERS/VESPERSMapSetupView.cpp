/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "VESPERSMapSetupView.h"

#include <QPushButton>
#include <QGroupBox>
#include <QProcess>
#include <QTextEdit>
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QCheckBox>

VESPERSMapSetupView::VESPERSMapSetupView(VESPERSMapSetup *mapSetup, QWidget *parent)
	: QWidget(parent)
{
	mapSetup_ = mapSetup;

	dim2D_ = new QRadioButton("2D");
	dim3D_ = new QRadioButton("3D");
	QButtonGroup *dimGroup = new QButtonGroup;
	dimGroup->addButton(dim2D_);
	dimGroup->addButton(dim3D_);
	dimGroup->setExclusive(true);
	connect(mapSetup_, SIGNAL(dimensionChanged(bool)), this, SLOT(onDimensionChanged(bool)));
	connect(dim2D_, SIGNAL(toggled(bool)), this, SLOT(setDimension(bool)));

	QVBoxLayout *dimLayout = new QVBoxLayout;
	dimLayout->addWidget(dim2D_, 0, Qt::AlignCenter);
	dimLayout->addWidget(dim3D_, 0, Qt::AlignCenter);
	QGroupBox *dimBox = new QGroupBox("Map Dimension");
	dimBox->setLayout(dimLayout);

	vortex1El_ = new QRadioButton("1-el Vortex");
	vortex4El_ = new QRadioButton("4-el Vortex");
	QButtonGroup *detectorGroup = new QButtonGroup;
	detectorGroup->addButton(vortex1El_);
	detectorGroup->addButton(vortex4El_);
	detectorGroup->setExclusive(true);
	connect(mapSetup_, SIGNAL(detectorChanged(bool)), this, SLOT(onDetectorChanged(bool)));
	connect(vortex1El_, SIGNAL(toggled(bool)), this, SLOT(setDetector(bool)));

	QVBoxLayout *vortexLayout = new QVBoxLayout;
	vortexLayout->addWidget(vortex1El_, 0, Qt::AlignCenter);
	vortexLayout->addWidget(vortex4El_, 0, Qt::AlignCenter);
	QGroupBox *vortexBox = new QGroupBox("XRF Detector Choice");
	vortexBox->setLayout(vortexLayout);

	QCheckBox *ccd = new QCheckBox("Use Roper CCD");
	connect(mapSetup_, SIGNAL(ccdBeingUsed(bool)), ccd, SLOT(setChecked(bool)));
	connect(ccd, SIGNAL(toggled(bool)), mapSetup_, SLOT(useCCD(bool)));

	QCheckBox *multipleImages = new QCheckBox("Multiple Images per File");
	multipleImages->setEnabled(false);
	connect(mapSetup_, SIGNAL(multipleImagesUsed(bool)), multipleImages, SLOT(setChecked(bool)));
	connect(multipleImages, SIGNAL(toggled(bool)), mapSetup_, SLOT(useMultiImages(bool)));

	QVBoxLayout *ccdLayout = new QVBoxLayout;
	ccdLayout->addWidget(ccd);
	ccdLayout->addWidget(multipleImages);
	QGroupBox *ccdBox = new QGroupBox("CCD options");
	ccdBox->setLayout(ccdLayout);

	QPushButton *start = new QPushButton("Launch Mapping Software");
	connect(start, SIGNAL(clicked()), mapSetup_, SLOT(launchNDMapper()));

	QVBoxLayout *optionsLayout = new QVBoxLayout;
	optionsLayout->addWidget(dimBox);
	optionsLayout->addWidget(vortexBox);
	optionsLayout->addWidget(ccdBox);
	optionsLayout->addWidget(start);

	QTextEdit *message = new QTextEdit;
	message->setPlainText(QString("This launches the XRF mapping software used on the VESPERS beamline.  It will launch the appropriate version based on the "
									   "selections of different options found to the left.\n\nThe choices of dimension are either a 2D map or a 3D map.  Note that "
									   "a 3D map requires the CCD to be enabled.\n\nThe choice of XRF detector is primarily based on the geometry of the endstation "
									   "and the beamline staff will instruct you on which detector you will be using for your experiment.\n\nIf you are also doing "
									   "a diffraction map, check off the Use Ropper CCD check box.  Currently the Multiple Images per File check box is under "
									   "development and should be left un-checked for now.\n\nOnce everything is setup, click the Launch Mapping Software button."));
	message->setReadOnly(true);

	QHBoxLayout *setupLayout = new QHBoxLayout;
	setupLayout->addStretch();
	setupLayout->addLayout(optionsLayout);
	setupLayout->addWidget(message);
	setupLayout->addStretch();

	QVBoxLayout *masterLayout = new QVBoxLayout;
	masterLayout->addStretch();
	masterLayout->addLayout(setupLayout);
	masterLayout->addStretch();

	setLayout(masterLayout);

	// Some auto connections based on choices.
	connect(dim3D_, SIGNAL(toggled(bool)), ccd, SLOT(setChecked(bool)));
	connect(dim3D_, SIGNAL(toggled(bool)), ccd, SLOT(setDisabled(bool)));
	connect(ccd, SIGNAL(toggled(bool)), multipleImages, SLOT(setEnabled(bool)));

	dim2D_->setChecked(true);
	vortex1El_->setChecked(true);
}
