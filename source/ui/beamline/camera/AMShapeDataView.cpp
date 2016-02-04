/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

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


#include "AMShapeDataView.h"

#include <QBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <cmath>
#include <QLabel>
#include <QScrollArea>

#include "beamline/camera/AMShapeData.h"
#include "ui/dataman/AMSampleView.h"
#include "util/AMErrorMonitor.h"

AMShapeDataView::AMShapeDataView(AMShapeData *shapeModel, QWidget *parent) :
	QWidget(parent)
{
	if(shapeModel == 0)
		shapeModel_ = new AMShapeData();
	else
		shapeModel_ = shapeModel;


	coordinateEdit_ = 0;


	/// Set up GUI

	QFrame* coordinateFrame= new QFrame();
	QHBoxLayout* ihl = new QHBoxLayout();
	ihl->setContentsMargins(12,4,12,4);
	ihl->addSpacing(10);
	ihl->addWidget(xEdit_ = new QLineEdit());
	ihl->addSpacing(10);
	ihl->addWidget(yEdit_ = new QLineEdit());
	ihl->addSpacing(10);
	ihl->addWidget(zEdit_ = new QLineEdit());
	ihl->addStretch();
	coordinateFrame->setLayout(ihl);

	QFrame* rotationFrame = new QFrame();
	QHBoxLayout *rotationLayout = new QHBoxLayout();
	rotationLayout->setContentsMargins(12,4,12,4);
	rotationLayout->addWidget(tiltEdit_ = new QLineEdit());
	rotationLayout->addSpacing(20);
	rotationLayout->addWidget(yRotationEdit_ = new QLineEdit());
	rotationLayout->addSpacing(20);
	rotationLayout->addWidget(rotationEdit_ = new QLineEdit());
	rotationLayout->addStretch();
	rotationFrame->setLayout(rotationLayout);

	QFrame* sliderFrame = new QFrame();
	QHBoxLayout* sliderLayout = new QHBoxLayout();
	sliderLayout->setContentsMargins(12,4,12,4);
	sliderLayout->addWidget(xAxisSlider_ = new QSlider(Qt::Horizontal));
	sliderLayout->addSpacing(20);
	sliderLayout->addWidget(yAxisSlider_ = new QSlider(Qt::Horizontal));
	sliderLayout->addSpacing(20);
	sliderLayout->addWidget(zAxisSlider_ = new QSlider(Qt::Horizontal));
	sliderLayout->addSpacing(20);
	sliderLayout->addWidget(showHideButton_ = new QPushButton("Show/Hide"));
	sliderLayout->addStretch();
	sliderFrame->setLayout(sliderLayout);
	xAxisSlider_->setRange(-250,250);
	xAxisSlider_->setValue(0);
	yAxisSlider_->setRange(-250,250);
	yAxisSlider_->setValue(0);
	zAxisSlider_->setRange(-250,250);
	zAxisSlider_->setValue(0);

	coordinateFrame_ = new QFrame();
	scrollArea_ = new QScrollArea();
	scrollArea_->setWidget(coordinateFrame_);
	scrollArea_->setWidgetResizable(true);


	QVBoxLayout* infoLayout = new QVBoxLayout();
	infoLayout->setContentsMargins(0,0,0,0);
	infoLayout->addWidget(coordinateFrame);
	infoLayout->addWidget(rotationFrame);
	infoLayout->addWidget(sliderFrame);
	infoLayout->addSpacing(2);
	infoLayout->addWidget(scrollArea_);
	infoLayout->addStretch();

	setLayout(infoLayout);
	setWindowTitle("Shape View");
	/// Make Connections

	connect(tiltEdit_, SIGNAL(textEdited(QString)), this, SLOT(tiltChanged(QString)));
	connect(rotationEdit_, SIGNAL(textEdited(QString)), this, SLOT(rotationChanged(QString)));
	connect(yRotationEdit_, SIGNAL(textEdited(QString)), this, SLOT(yAxisRotationChanged(QString)));

	connect(xEdit_, SIGNAL(textEdited(QString)), this, SLOT(xChanged(QString)));
	connect(yEdit_, SIGNAL(textEdited(QString)), this, SLOT(yChanged(QString)));
	connect(zEdit_, SIGNAL(textEdited(QString)), this, SLOT(zChanged(QString)));

	connect(xAxisSlider_, SIGNAL(sliderMoved(int)), this, SLOT(xAxisRotation(int)));
	connect(yAxisSlider_, SIGNAL(sliderMoved(int)), this, SLOT(yAxisRotation(int)));
	connect(zAxisSlider_, SIGNAL(sliderMoved(int)), this, SLOT(zAxisRotation(int)));


	connect(showHideButton_, SIGNAL(clicked()), this, SLOT(toggleShapeVisible()));
	connect(shapeModel_, SIGNAL(shapeDataChanged(AMShapeData*)), this, SLOT(updateAll()));
	updateAll();

}



void AMShapeDataView::setTilt(QString tilt)
{
	tiltEdit_->setText(tilt);
}

void AMShapeDataView::setX(QString x)
{
	xEdit_->setText(x);
}

void AMShapeDataView::setY(QString y)
{
	yEdit_->setText(y);
}

void AMShapeDataView::setZ(QString z)
{
	zEdit_->setText(z);
}

void AMShapeDataView::setRotation(QString rotation)
{
	rotationEdit_->setText(rotation);
}

void AMShapeDataView::setShapeData(AMShapeData *shapeData)
{
	if(shapeModel_ != shapeData)
	{
		disconnect(shapeModel_, SIGNAL(shapeDataChanged()), this, SLOT(updateAll()));
		shapeModel_ = shapeData;
		connect(shapeModel_, SIGNAL(shapeDataChanged()), this, SLOT(updateAll()));
		updateAll();
	}
}

void AMShapeDataView::setYAxisRotation(QString rotation)
{
	yRotationEdit_->setText(rotation);
}

void AMShapeDataView::nameChanged(QString name)
{
	if(isValid())
	{
		shapeModel_->setName(name);
		emit newName();
	}
}

void AMShapeDataView::tiltChanged(QString tilt)
{
	if(isValid())
	{
		shapeModel_->setTilt(tilt.toDouble());
		bool block = true;
		block = xAxisSlider_->blockSignals(block);
		int value = int(tilt.toDouble()/M_PI*xAxisSlider_->maximum());
		xAxisSlider_->setValue(value);
		xAxisSlider_->blockSignals(block);
		emit updateShapes();
	}
}

void AMShapeDataView::xChanged(QString xString)
{

	if(isValid())
	{
		double x = xString.toDouble();
		QVector3D coordinate = shapeModel_->centerCoordinate();
		coordinate.setX(x);
		shapeModel_->shiftTo(coordinate);
		emit updateShapes();
	}
}

void AMShapeDataView::yChanged(QString yString)
{
	if(isValid())
	{
		double y = yString.toDouble();
		QVector3D coordinate = shapeModel_->centerCoordinate();
		coordinate.setY(y);
		shapeModel_->shiftTo(coordinate);
		emit updateShapes();
	}
}

void AMShapeDataView::zChanged(QString zString)
{
	if(isValid())
	{
		double z = zString.toDouble();
		QVector3D coordinate = shapeModel_->centerCoordinate();
		coordinate.setZ(z);
		shapeModel_->shiftTo(coordinate);
		emit updateShapes();
	}
}


void AMShapeDataView::rotationChanged(QString rotation)
{
	if(isValid())
	{
		shapeModel_->setRotation(rotation.toDouble());
		bool block = true;
		block = zAxisSlider_->blockSignals(block);
		int value = int(rotation.toDouble()/M_PI*zAxisSlider_->maximum());
		zAxisSlider_->setValue(value);
		zAxisSlider_->blockSignals(block);
		emit updateShapes();
	}
}

void AMShapeDataView::yAxisRotationChanged(QString rotation)
{
	if(isValid())
	{
		shapeModel_->setYAxisRotation(rotation.toDouble());
		bool block = true;
		block = yAxisSlider_->blockSignals(block);
		int value = int(rotation.toDouble()/M_PI*yAxisSlider_->maximum());
		yAxisSlider_->setValue(value);
		yAxisSlider_->blockSignals(block);
		emit updateShapes();


	}
}

void AMShapeDataView::toggleShapeVisible()
{
	bool visible = !shapeModel_->visible();
	setShapeVisible(visible);
	emit updateShapes();
}

void AMShapeDataView::setShapeVisible(bool visible)
{
	shapeModel_->setVisible(visible);
}

void AMShapeDataView::showSampleView()
{
}

void AMShapeDataView::setCoordinate()
{
	QVector3D newCoordinate;
	for(int i = 0; i < count(); i++)
	{
		newCoordinate.setX(coordinateEdit_[3*i]->text().toDouble());
		newCoordinate.setY(coordinateEdit_[3*i+1]->text().toDouble());
		newCoordinate.setZ(coordinateEdit_[3*i+2]->text().toDouble());
		shapeModel_->setCoordinate(newCoordinate,i);
		if(i == 0)
		{
			shapeModel_->setCoordinate(newCoordinate,count());
		}
	}
	emit updateShapes();
}


void AMShapeDataView::xAxisRotation(int value)
{
	double max = xAxisSlider_->maximum();
	double tilt = value/max*M_PI;
	shapeModel_->setTilt(tilt);
	bool block = tiltEdit_->blockSignals(true);
	tiltEdit_->setText(QString::number(tilt));
	tiltEdit_->blockSignals(block);
	emit updateShapes();

}

void AMShapeDataView::yAxisRotation(int value)
{
	double max = yAxisSlider_->maximum();
	double rotation = value/max*M_PI;
	shapeModel_->setYAxisRotation(rotation);
	bool block = yRotationEdit_->blockSignals(true);
	yRotationEdit_->setText(QString::number(rotation));
	yRotationEdit_->blockSignals(block);
	emit updateShapes();
}

void AMShapeDataView::zAxisRotation(int value)
{
	double max = zAxisSlider_->maximum();
	double rotation = value/max*M_PI;
	shapeModel_->setRotation(rotation);
	bool block = rotationEdit_->blockSignals(true);
	rotationEdit_->setText(QString::number(rotation));
	rotationEdit_->blockSignals(block);
	emit updateShapes();
}


void AMShapeDataView::updateAll()
{
	if(isValid())
	{
		updateCoordinateLabels();
		if(!tiltEdit_->hasFocus())
			tiltEdit_->setText(QString("%1").arg(shapeModel_->tilt()));
		QVector3D coordinate = shapeModel_->centerCoordinate();
		if(!xEdit_->hasFocus())
			xEdit_->setText(QString::number(coordinate.x()));
		if(!yEdit_->hasFocus())
			yEdit_->setText(QString::number(coordinate.y()));
		if(!zEdit_->hasFocus())
			zEdit_->setText(QString::number(coordinate.z()));
		if(!rotationEdit_->hasFocus())
			rotationEdit_->setText(QString::number(shapeModel_->rotation()));
		if(!yRotationEdit_->hasFocus())
			yRotationEdit_->setText(QString::number(shapeModel_->yAxisRotation()));
	}
	else
		AMErrorMon::alert(this, AMSHAPEDATAVIEW_UPDATEALL_FAILED_NOT_VALID, QString("Failed to update AMShapeDataView, view is not currently valid.") );
}

bool AMShapeDataView::isValid()
{
	if(shapeModel_ == 0)
		return false;
	else if(shapeModel_->count() <= 0)
	{
		return false;
	}
	return true;
}

void AMShapeDataView::updateCoordinateLabels()
{
	int points = count();// ignore the closing point of the shape
	if(points != oldCount_)
	{
		coordinateFrame_->deleteLater();
		coordinateFrame_ = new QFrame();
		if(coordinateEdit_)
		{
			delete [] coordinateEdit_;
		}

		coordinateEdit_ = new QLineEdit*[points*3];
		QVBoxLayout* verticalLayout= new QVBoxLayout();
		verticalLayout->setContentsMargins(0,0,0,0);
		QFrame* pointFrame [points];
		QHBoxLayout* pointLayout [points];
		for(int i = 0; i < points; i++)
		{
			QString labelText;
			if(4 == points)
			{
				switch(i)
				{
				case 0:
					labelText = "Top Left";
					break;
				case 1:
					labelText = "Top Right";
					break;
				case 2:
					labelText = "Bottom Right";
					break;
				case 3:
					labelText = "Bottom Left";
					break;
				default:
					labelText = QString("%1").arg(i);
				}
			}
			else
				labelText = QString("%1").arg(i);

			pointFrame[i] = new QFrame();
			pointLayout[i] = new QHBoxLayout();
			pointLayout[i]->setContentsMargins(12,4,12,4);
			pointLayout[i]->addWidget(coordinateEdit_[3*i] = new QLineEdit(QString("%1").arg(shapeModel_->coordinate(i).x())));
			pointLayout[i]->addWidget(coordinateEdit_[3*i+1] = new QLineEdit(QString("%1").arg(shapeModel_->coordinate(i).y())));
			pointLayout[i]->addWidget(coordinateEdit_[3*i+2] = new QLineEdit(QString("%1").arg(shapeModel_->coordinate(i).z())));
			pointLayout[i]->addWidget(new QLabel(labelText));
			pointFrame[i]->setLayout(pointLayout[i]);
			verticalLayout->addWidget(pointFrame[i]);
			for(int n = 0; n < 3; n++)
				connect(coordinateEdit_[3*i+n], SIGNAL(textEdited(QString)), this, SLOT(setCoordinate()));
		}
		verticalLayout->addStretch();
		coordinateFrame_->setLayout(verticalLayout);
		scrollArea_->setWidget(coordinateFrame_);
		scrollArea_->setMaximumSize(coordinateFrame_->size());
	}
	else
	{
		for(int i = 0; i < points; i ++)
		{
			if(!coordinateEdit_[3*i]->hasFocus())
				coordinateEdit_[3*i]->setText(QString("%1").arg(shapeModel_->coordinate(i).x()));
			if(!coordinateEdit_[3*i+1]->hasFocus())
				coordinateEdit_[3*i+1]->setText(QString("%1").arg(shapeModel_->coordinate(i).y()));
			if(!coordinateEdit_[3*i+2]->hasFocus())
				coordinateEdit_[3*i+2]->setText(QString("%1").arg(shapeModel_->coordinate(i).z()));
		}
	}
	oldCount_ = points;
}

int AMShapeDataView::count()
{
	if(shapeModel_)
		return shapeModel_->count()-1;
	else return 0;
}




