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


#ifndef AMSIMPLEBEAMCONFIGURATIONWIZARD_H
#define AMSIMPLEBEAMCONFIGURATIONWIZARD_H

#include "AMGraphicsViewWizard.h"



/** The AMSimpleBeamConfigurationWizard is the wizard for
  * configuring the beam location.  The configuration in
  * this wizard is done by dragging beam spot markers
  * over top of the visible beam.  Visible beam needs to be
  * turned on to do this
  */

class AMSimpleBeamConfigurationWizard : public AMGraphicsViewWizard
{
	Q_OBJECT
public:
	enum
	{
		Page_Intro = 0, // description
		Page_Final = 1, // confirmation of completion
		Page_Option = 2, // set coordinates
		Page_Free = 3 // for wizard use

	};
	AMSimpleBeamConfigurationWizard(QWidget *parent = 0);
	virtual ~AMSimpleBeamConfigurationWizard();
	virtual int nextId() const;
	/// waitPage overwrites the AMGraphicsViewWizard waitPage
	virtual void waitPage();
	virtual QString message(int type);

public slots:
	void back();
	void initBeamShape();
	void mousePressedHandler(QPointF);
	void moveBeamShapeHandler(QPointF);

signals:
	void createBeamShape(int);
	void mousePressed(QPointF, int);
	void moveBeamShape(QPointF, int);

};

/** AMSimpleBeamConfigurationSetPage is a page for setting
  * a particular beam marker in the AMSimpleBeamConfigurationWizard.
  */

class AMSimpleBeamConfigurationSetPage : public AMViewPage
{
	Q_OBJECT
public:
	void initializePage();
signals:
	void initBeamShape();
	void signalMousePressed(QPointF);
	void moveBeamShape(QPointF);

protected slots:
	void selectShape(QPointF);
	void releaseShape();
	void moveShape(QPointF);
	void disconnectMouseSignal();



};

class AMSimpleBeamConfigurationWaitPage : public AMWaitPage
{
	Q_OBJECT
public:
	/// This sets up the page and requests the move (by calling waitPage())
	void initializePage();
};

#endif // AMSIMPLEBEAMCONFIGURATIONWIZARD_H
