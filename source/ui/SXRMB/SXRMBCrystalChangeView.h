#ifndef SXRMBCRYSTALCHANGEVIEW_H
#define SXRMBCRYSTALCHANGEVIEW_H

#include <QWidget>

#include "beamline/SXRMB/SXRMBCrystalChangeModel.h"
#include "ui/beamline/AMExtendedControlEditor.h"

/// This class visualizes the crystal selection operations.
class SXRMBCrystalChangeView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit SXRMBCrystalChangeView(SXRMBCrystalChangeModel *crystalChange, QWidget *parent = 0);
	/// Destructor.
	~SXRMBCrystalChangeView();

signals:

public slots:

protected slots:
	/// Handles changing the enabled state of the other controls when the crystal is changing.
	void onCrystalSelectionChanged();

protected:
	/// The crystal selection model.
	SXRMBCrystalChangeModel *crystalChange_;

	/// The control editor for the crystal selection.
	AMExtendedControlEditor *crystalSelectionEditor_;
	/// The control editor for the crystal Y position.
	AMExtendedControlEditor *crystalYEditor_;
	/// The control editor for the crystal theta position.
	AMExtendedControlEditor *crystalThetaEditor_;
	/// The control editor for the crystal chi position.
	AMExtendedControlEditor *crystalChiEditor_;
};

#endif // SXRMBCRYSTALCHANGEVIEW_H
