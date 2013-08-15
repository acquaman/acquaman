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


#ifndef AMBEAMLINESAMPLEPLATEPRE2013MOVEACTION_H
#define AMBEAMLINESAMPLEPLATEPRE2013MOVEACTION_H

#include "AMBeamlineControlSetMoveAction.h"

#define AMBEAMLINEACTIONITEM_SAMPLE_ID_NOT_ON_PLATE 27300

class AMSamplePlatePre2013ItemModel;

class AMBeamlineSamplePlatePre2013MoveAction : public AMBeamlineControlSetMoveAction
{
Q_OBJECT
public:
	AMBeamlineSamplePlatePre2013MoveAction(int sampleID, AMSamplePlatePre2013ItemModel *plateModel, QObject *parent = 0);

	virtual AMBeamlineActionItemView* createView(int index = 0);

	virtual AMBeamlineActionItem* createCopy() const;

	QString sampleDescription() const;

protected slots:
	void onSamplePlateDataChanged(QModelIndex topLeft, QModelIndex bottomRight);

protected:
	int sampleID_;
	AMSamplePlatePre2013ItemModel *samplePlateModel_;
	QString sampleDescription_;
};

class AMBeamlineSamplePlatePre2013MoveActionView : public AMBeamlineControlSetMoveActionView
{
Q_OBJECT
public:
	AMBeamlineSamplePlatePre2013MoveActionView(AMBeamlineSamplePlatePre2013MoveAction *samplePlateAction, int index = 0, QWidget *parent = 0);

protected slots:
	void onInfoChanged();

protected:
	AMBeamlineSamplePlatePre2013MoveAction *samplePlateAction_;
};

#endif // AMBEAMLINESAMPLEPLATEPRE2013MOVEACTION_H
