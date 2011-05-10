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
