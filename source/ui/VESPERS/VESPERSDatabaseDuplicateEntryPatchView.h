#ifndef VESPERSDATABASEDUPLICATEENTRYPATCHVIEW_H
#define VESPERSDATABASEDUPLICATEENTRYPATCHVIEW_H

#include <QWidget>

#include "dataman/VESPERS/VESPERSDatabaseDuplicateEntryPatch.h"

/// Very simple class that gets everything going.
class VESPERSDatabaseDuplicateEntryPatchView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes the patch object.
	explicit VESPERSDatabaseDuplicateEntryPatchView(VESPERSDatabaseDuplicateEntryPatch *patch, QWidget *parent = 0);

signals:

public slots:

protected:
	/// Holding the pointer to the patch.
	VESPERSDatabaseDuplicateEntryPatch *patch_;
};

#endif // VESPERSDATABASEDUPLICATEENTRYPATCHVIEW_H
