#ifndef SXRMBCRYSTALCHANGEVIEW_H
#define SXRMBCRYSTALCHANGEVIEW_H

#include <QWidget>

#include "beamline/SXRMB/SXRMBCrystalChangeModel.h"

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

protected:
	/// The crystal selection model.
	SXRMBCrystalChangeModel *crystalChange_;
};

#endif // SXRMBCRYSTALCHANGEVIEW_H
