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


#ifndef AMSAMPLEPLATEWIZARD_H
#define AMSAMPLEPLATEWIZARD_H

#include "AMGraphicsViewWizard.h"

class QSlider;
class QGraphicsRectItem;

#define AMSAMPLEPLATEWIZARD_NEXTID_ERROR_FOR_WAITPAGE 602001
#define AMSAMPLEPLATEWIZARD_NEXTID_ERROR_FOR_SETPAGE 602002
#define AMSAMPLEPLATEWIZARD_TRIGGER_RESET_FROM_INVALID_PAGE 602003

class AMSamplePlateWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum
	{
		Page_Intro = 0, // description -> should skip on start
		Page_Check = 1,  // check to see if the plate is where it should be
		Page_Final = 2,
		Page_Option = 3,
		Page_Free = 4
	};
	AMSamplePlateWizard(QWidget* parent = 0);
	virtual ~AMSamplePlateWizard();

	virtual int nextId() const;

	virtual void waitPage();

	virtual QString message(int type);

	double currentRotation();

protected:

	double requestMotorRotation();



public slots:
	void back();
	virtual void addPoint(QPointF position);

	/// Will add duplicate points unless this is called when leaving the page
	void removePoint(QPointF* point);

	void setCurrentRotation(double rotation);



signals:
	void movePlate(int);
	void requestRotation();

protected:
	double coordinateX(int id);
	double coordinateY(int id);
	double coordinateZ(int id);
	void addResetPointsButton(int id);
	QPen getDefaultPen();

protected slots:
	void triggerReset(int id);
	void addResetPage(int id);
	void showResetButton(int id);

private:
	QVector<int> resetPages_;
	QVector<QGraphicsRectItem*> samplePointShapes_;
	double currentRotation_;


};

class AMSampleIntroPage : public AMWizardPage
{
	Q_OBJECT
public:
	void initializePage();

	void timerEvent(QTimerEvent *event);

};

class AMSampleWaitPage : public AMWaitPage
{
	Q_OBJECT
public:
	void initializePage();
};

class AMSampleSetPage : public AMViewPage
{
	Q_OBJECT
public:
	//    AMSampleSetPage(QWidget* parent = 0);
	void initializePage();

public slots:

	void addPoint(QPointF position);
	/// add a point to the page's list of points
	void insertPoint(QPointF *position);
	void disconnectMouseSignal();
	/// clears all the points currently selected by the page.
	void resetPoints();

private:
	/// these are the points associated with a particular page.
	QVector<QPointF*> localPoints_;
};

class AMSampleCheckPage : public AMCheckPage
{
	Q_OBJECT
protected slots:
	virtual void checkBoxChanged(bool state);
};

#endif // AMSAMPLEPLATEWIZARD_H
