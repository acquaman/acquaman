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


#ifndef AMROTATIONWIZARD_H
#define AMROTATIONWIZARD_H

#include "AMGraphicsViewWizard.h"

/// The AMRotationWizard is used to calculate the centre of rotation
class AMRotationWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum
	{
		Page_Intro,
		Page_Check,
		Page_Final,
		Page_Option,
		Page_Free
	};
	AMRotationWizard(QWidget* parent = 0);
	virtual ~AMRotationWizard();
	int nextId() const;

	void waitPage();

	virtual QString message(int type);

public slots:
	void back();
	virtual void addPoint(QPointF position);


};

class AMRotationWaitPage: public AMWaitPage
{
	Q_OBJECT
public:
	void initializePage();
};

class AMRotationSetPage: public AMViewPage
{
	Q_OBJECT
public:
	virtual void initializePage();
	virtual bool validatePage();
	virtual void cleanupPage();

public slots:
	void addPoint(QPointF position);

protected:
	void disconnectSignals();
};

class AMRotationCheckPage : public AMCheckPage
{
	Q_OBJECT
protected slots:
	virtual void checkBoxChanged(bool state);
};

#endif // AMROTATIONWIZARD_H
