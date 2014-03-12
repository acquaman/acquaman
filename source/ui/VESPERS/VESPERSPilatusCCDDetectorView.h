#ifndef VESPERSPILATUSCCDDETECTORVIEW_H
#define VESPERSPILATUSCCDDETECTORVIEW_H

#include "ui/VESPERS/VESPERSCCDDetectorView.h"
#include "beamline/VESPERS/VESPERSPilatusCCDDetector.h"

#include <QProgressBar>

/// This class extends the basic detector view and adds in some file system information and enables copying all the images to the aurora drive.
class VESPERSPilatusCCDDetectorView : public VESPERSCCDDetectorView
{
	Q_OBJECT

public:
	/// Constructor.  Extends the standard CCD detector view.
 	virtual ~VESPERSPilatusCCDDetectorView();
	Q_INVOKABLE explicit VESPERSPilatusCCDDetectorView(VESPERSPilatusCCDDetector *detector, QWidget *parent = 0);

protected slots:
	/// Updates the progress bar and status label when new disk usage data is read.
	void onFileSystemInfoUpdate();

protected:
	/// A progresss bar that shows the disk usage.
	QProgressBar *diskUsageBar_;
	/// Label that has a light in it to indicate relative fullness.  Green from 0-60%, yellow from 60-80%, and red for >80%.
	QLabel *status_;
	/// Label that holds the ratio of disk space occupied vs total.
	QLabel *spaceLabel_;
};

#endif // VESPERSPILATUSCCDDETECTORVIEW_H
