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


#ifndef AMSIMPLESAMPLEPLATEWIZARD_H
#define AMSIMPLESAMPLEPLATEWIZARD_H

#include "AMGraphicsViewWizard.h"

class AMShapeData;
class QVector3D;
class QPointF;


/** AMSimpleSamplePlateWizard is a wizard for configuring
  * the sample plate.  It works by requesting a sample plate
  * that must be moved to the correct position.  It basically
  * functions as an interface to move the sample plate.
  */

class AMSimpleSamplePlateWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum
	{
		Page_Intro = 0, // description
		Page_Final = 1, // confirmation
		Page_Option = 2, // set coordinate
		Page_Free = 3 // for wizard use
	};
	AMSimpleSamplePlateWizard(QWidget *parent = 0);
	virtual ~AMSimpleSamplePlateWizard();
	virtual int nextId() const;
	virtual void waitPage();
	virtual QString message(int type);

public slots:
	void back();
	void addShape(AMShapeData *shape);
	void shiftSampleShape(QPointF shift);
	void initializeShape();
	void setPageMousePressedHandler(QPointF);
signals:
	void signalInitializeSampleShape();
	void signalShiftSampleShape(QPointF);
	void signalSamplePlateWizardMousePressed(QPointF);
private:
};

class AMSimpleSampleSetPage : public AMViewPage
{
	Q_OBJECT
public:
	void initializePage();
protected slots:
	void selectShape(QPointF);
	void releaseShape();
	void moveShape(QPointF);
	void disconnectMouseSignal();

signals:
	void signalMousePressed(QPointF);

private:
	QPointF oldPosition_;
};

class AMSimpleSampleWaitPage : public AMWaitPage
{
	Q_OBJECT
public:
	virtual void initializePage();
};

#endif // AMSIMPLESAMPLEPLATEWIZARD_H
