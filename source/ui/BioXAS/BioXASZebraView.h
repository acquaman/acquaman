#ifndef BIOXASZEBRAVIEW_H
#define BIOXASZEBRAVIEW_H

#include <QWidget>

#include "beamline/BioXAS/BioXASZebra.h"

/// This is the main widget view for looking at the Zebra in Acquaman.
class BioXASZebraView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes a zebra widget.
	explicit BioXASZebraView(BioXASZebra *zebra, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraView();

signals:

public slots:

protected:
	/// The zebra model.
	BioXASZebra *zebra_;
};

#endif // BIOXASZEBRAVIEW_H
