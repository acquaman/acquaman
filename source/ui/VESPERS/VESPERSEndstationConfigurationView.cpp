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


#include "VESPERSEndstationConfigurationView.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmapCache>
#include <QStringBuilder>

#include "ui/AMTopFrame.h"

VESPERSEndstationConfigurationView::VESPERSEndstationConfigurationView(VESPERSEndstationConfiguration *config, QWidget *parent)
	: QWidget(parent)
{
	endstation_ = config;

	AMTopFrame *topFrame = new AMTopFrame("Endstation Configuration");

	// Setting up the geometries.
	geometry_ = new QButtonGroup;
	geometry_->setExclusive(true);
	QVBoxLayout *geometryLayout = new QVBoxLayout;

	QCheckBox *tempBox = new QCheckBox("Straight On");
	geometry_->addButton(tempBox, 1);
	geometryLayout->addWidget(tempBox);
	tempBox = new QCheckBox(QString("Vertical 45%1").arg(QString::fromUtf8("°")));
	geometry_->addButton(tempBox, 2);
	geometryLayout->addWidget(tempBox);
	tempBox = new QCheckBox(QString("Horizontal 45%1").arg(QString::fromUtf8("°")));
	geometry_->addButton(tempBox, 3);
	geometryLayout->addWidget(tempBox);
	tempBox = new QCheckBox(QString("Double 45%1").arg(QString::fromUtf8("°")));
	geometry_->addButton(tempBox, 4);
	geometryLayout->addWidget(tempBox);
	tempBox = new QCheckBox("Big Beam");
	geometry_->addButton(tempBox, 5);
	geometryLayout->addWidget(tempBox);

	geometryBox_ = new QGroupBox("Geometry");
	geometryBox_->setLayout(geometryLayout);

	connect(geometry_, SIGNAL(buttonClicked(int)), this, SLOT(onGeometryClicked(int)));

	// Setting up the techniques.
	techniques_ = new QButtonGroup;
	techniques_->setExclusive(false);
	QVBoxLayout *techniquesLayout = new QVBoxLayout;

	xas_ = new QCheckBox("X-ray Absorption Spectroscopy");
	techniques_->addButton(xas_, 0);
	techniquesLayout->addWidget(xas_);
	xrf_ = new QCheckBox("X-Ray Fluorescence");
	techniques_->addButton(xrf_, 1);
	techniquesLayout->addWidget(xrf_);
	xrd_ = new QCheckBox("X-Ray Diffraction");
	techniques_->addButton(xrd_, 2);
	techniquesLayout->addWidget(xrd_);

	techniquesBox_ = new QGroupBox("Techniques");
	techniquesBox_->hide();
	techniquesBox_->setLayout(techniquesLayout);

	connect(techniques_, SIGNAL(buttonClicked(int)), this, SLOT(onTechniquesClicked(int)));

	// Setting up the detectors.
	detectors_ = new QButtonGroup;
	detectors_->setExclusive(false);
	QVBoxLayout *detectorsLayout = new QVBoxLayout;

	ionChambers_ = new QCheckBox("Ion Chambers");
	detectors_->addButton(ionChambers_, 0);
	detectorsLayout->addWidget(ionChambers_);
	vortex1E_ = new QCheckBox("Single Element Vortex");
	detectors_->addButton(vortex1E_, 1);
	detectorsLayout->addWidget(vortex1E_);
	vortex4E_ = new QCheckBox("Four Element Vortex");
	detectors_->addButton(vortex4E_, 2);
	detectorsLayout->addWidget(vortex4E_);
	roperCCD_ = new QCheckBox("Roper CCD");
	detectors_->addButton(roperCCD_, 3);
	detectorsLayout->addWidget(roperCCD_);

	detectorsBox_ = new QGroupBox("Detectors");
	detectorsBox_->hide();
	detectorsBox_->setLayout(detectorsLayout);

	connect(detectors_, SIGNAL(buttonClicked(int)), this, SLOT(onDetectorsClicked(int)));

	// The picture.
	cartoon_ = new QLabel;
	cartoon_->setPixmap(endstationPixmap("endstation-unselected"));

	QHBoxLayout *boxLayout = new QHBoxLayout;
	boxLayout->addStretch();
	boxLayout->addWidget(geometryBox_);
	boxLayout->addWidget(techniquesBox_);
	boxLayout->addWidget(detectorsBox_);
	boxLayout->addStretch();

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(topFrame);
	mainLayout->addStretch();
	mainLayout->addLayout(boxLayout);
	mainLayout->addWidget(cartoon_, 0, Qt::AlignCenter);
	mainLayout->addStretch();

	setLayout(mainLayout);
}

void VESPERSEndstationConfigurationView::onGeometryClicked(int id)
{
	switch(id){

	case 1:	// Straight on.
		endstation_->setGeometry(VESPERSEndstationConfiguration::StraightOn);
		break;

	case 2:	// Vertical 45.
		endstation_->setGeometry(VESPERSEndstationConfiguration::Single45Vertical);
		break;

	case 3:	// Horizontal 45.
		endstation_->setGeometry(VESPERSEndstationConfiguration::Single45Horizontal);
		break;

	case 4:	// Double 45.
		endstation_->setGeometry(VESPERSEndstationConfiguration::Double45);
		break;

	case 5:	// Big beam.
		endstation_->setGeometry(VESPERSEndstationConfiguration::BigBeam);
		break;
	}

	updateAppearance();
	updatePixmap();

	if (techniquesBox_->isHidden())
		techniquesBox_->show();

	if (detectorsBox_->isHidden())
		detectorsBox_->show();
}

void VESPERSEndstationConfigurationView::onTechniquesClicked(int id)
{
	switch(id){

	case 0:	// XAS
		endstation_->setUsingXAS(techniques_->button(id)->isChecked());
		break;

	case 1:	// XRF
		endstation_->setUsingXRF(techniques_->button(id)->isChecked());
		break;

	case 2:	// XRD
		endstation_->setUsingXRD(techniques_->button(id)->isChecked());
		break;
	}
}

void VESPERSEndstationConfigurationView::onDetectorsClicked(int id)
{
	switch(id){

	case 0:	// Ion chambers.
		endstation_->setUsingIonChambers(detectors_->button(id)->isChecked());
		break;

	case 1:	// Single element vortex.
		endstation_->setUsingSingleElementVortex(detectors_->button(id)->isChecked());
		break;

	case 2:	// Four element vortex.
		endstation_->setUsingFourElementVortex(detectors_->button(id)->isChecked());
		break;

	case 3:	// Roper CCD.
		endstation_->setUsingRoperCCD(detectors_->button(id)->isChecked());
		break;
	}

	updatePixmap();
}

void VESPERSEndstationConfigurationView::updateAppearance()
{
	xas_->setEnabled(endstation_->canUseXAS());
	xrf_->setEnabled(endstation_->canUseXRF());
	xrd_->setEnabled(endstation_->canUseXRD());
	ionChambers_->setEnabled(endstation_->canUseIonChambers());
	vortex1E_->setEnabled(endstation_->canUseSingleElementVortex());
	vortex4E_->setEnabled(endstation_->canUseFourElementVortex());
	roperCCD_->setEnabled(endstation_->canUseRoperCCD());

	foreach(QAbstractButton *button, detectors_->buttons())
		button->setChecked(false);
}

void VESPERSEndstationConfigurationView::updatePixmap()
{
	switch(endstation_->geometry()){

	case VESPERSEndstationConfiguration::Invalid:

		cartoon_->setPixmap(endstationPixmap("endstation-unselected.png"));
		break;

	case VESPERSEndstationConfiguration::StraightOn:

		if (endstation_->usingIonChambers() && endstation_->usingSingleElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-straight-on-ionChambers-vortex1"));

		else if (endstation_->usingIonChambers())
			cartoon_->setPixmap(endstationPixmap("endstation-straight-on-ionChambers"));

		else if (endstation_->usingSingleElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-straight-on-vortex1"));

		else
			cartoon_->setPixmap(endstationPixmap("endstation-straight-on-unselected"));

		break;

	case VESPERSEndstationConfiguration::Single45Vertical:

		if (endstation_->usingIonChambers() && endstation_->usingSingleElementVortex() && endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-ionChambers-vortex1-roperCCD"));

		else if (endstation_->usingSingleElementVortex() && endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-vortex1-roperCCD"));

		else if (endstation_->usingIonChambers() && endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-ionChambers-roperCCD"));

		else if (endstation_->usingIonChambers() && endstation_->usingSingleElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-ionChambers-vortex1"));

		else if (endstation_->usingIonChambers())
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-ionChambers"));

		else if (endstation_->usingSingleElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-vortex1"));

		else if (endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-roperCCD"));

		else
			cartoon_->setPixmap(endstationPixmap("endstation-vertical45-unselected"));

		break;

	case VESPERSEndstationConfiguration::Single45Horizontal:

		if (endstation_->usingIonChambers() && endstation_->usingFourElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-horizontal45-ionChambers-vortex4"));

		else if (endstation_->usingIonChambers())
			cartoon_->setPixmap(endstationPixmap("endstation-horizontal45-ionChambers"));

		else if (endstation_->usingFourElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-horizontal45-vortex4"));

		else
			cartoon_->setPixmap(endstationPixmap("endstation-horizontal45-unselected"));

		break;

	case VESPERSEndstationConfiguration::Double45:

		if (endstation_->usingIonChambers() && endstation_->usingFourElementVortex() && endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-double45-ionChambers-vortex4-roperCCD"));

		else if (endstation_->usingFourElementVortex() && endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-double45-vortex4-roperCCD"));

		else if (endstation_->usingIonChambers() && endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-double45-ionChambers-roperCCD"));

		else if (endstation_->usingIonChambers() && endstation_->usingFourElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-double45-ionChambers-vortex4"));

		else if (endstation_->usingIonChambers())
			cartoon_->setPixmap(endstationPixmap("endstation-double45-ionChambers"));

		else if (endstation_->usingFourElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-double45-vortex4"));

		else if (endstation_->usingRoperCCD())
			cartoon_->setPixmap(endstationPixmap("endstation-double45-roperCCD"));

		else
			cartoon_->setPixmap(endstationPixmap("endstation-double45-unselected"));

		break;

	case VESPERSEndstationConfiguration::BigBeam:

		if (endstation_->usingIonChambers() && endstation_->usingFourElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-big-beam-ionChambers-vortex4"));

		else if (endstation_->usingIonChambers())
			cartoon_->setPixmap(endstationPixmap("endstation-big-beam-ionChambers"));

		else if (endstation_->usingFourElementVortex())
			cartoon_->setPixmap(endstationPixmap("endstation-big-beam-vortex4"));

		else
			cartoon_->setPixmap(endstationPixmap("endstation-big-beam-unselected"));

		break;
	}
}

QPixmap VESPERSEndstationConfigurationView::endstationPixmap(const QString &name) const
{
	QPixmap pixmap;

	if (!QPixmapCache::find("VESPERSEndstation" % name, &pixmap)){

		pixmap.load(":/VESPERS/" % name % ".png");
		pixmap = pixmap.scaled(600, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		QPixmapCache::insert("VESPERSEndstationIcon" % name, pixmap);
	}

	return pixmap;
}
