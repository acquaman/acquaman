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


#ifndef AMBEAMLINESAMPLEPLATEMOVEACTION_H
#define AMBEAMLINESAMPLEPLATEMOVEACTION_H

#include "AMBeamlineControlSetMoveAction.h"

#define AMBEAMLINEACTIONITEM_SAMPLE_ID_NOT_ON_PLATE 27300

class AMSamplePlateItemModel;

class AMBeamlineSamplePlateMoveAction : public AMBeamlineControlSetMoveAction
{
Q_OBJECT
public:
	AMBeamlineSamplePlateMoveAction(int sampleID, AMSamplePlateItemModel *plateModel, QObject *parent = 0);

	virtual AMBeamlineActionItemView* createView(int index = 0);

	virtual AMBeamlineActionItem* createCopy() const;

	QString sampleDescription() const;

protected slots:
	void onSamplePlateDataChanged(QModelIndex topLeft, QModelIndex bottomRight);

protected:
	int sampleID_;
	AMSamplePlateItemModel *samplePlateModel_;
	QString sampleDescription_;
};

class AMBeamlineSamplePlateMoveActionView : public AMBeamlineControlSetMoveActionView
{
Q_OBJECT
public:
	AMBeamlineSamplePlateMoveActionView(AMBeamlineSamplePlateMoveAction *samplePlateAction, int index = 0, QWidget *parent = 0);

protected slots:
	void onInfoChanged();

protected:
	AMBeamlineSamplePlateMoveAction *samplePlateAction_;
};

#endif // AMBEAMLINESAMPLEPLATEMOVEACTION_H
