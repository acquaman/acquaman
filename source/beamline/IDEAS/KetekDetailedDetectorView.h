#ifndef KETEKDETAILEDDETECTORVIEW_H
#define KETEKDETAILEDDETECTORVIEW_H

#include "ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h"
#include "beamline/IDEAS/IDEASKETEKDetector.h"

class KetekDetailedDetectorView : public IDEASXRFDetailedDetectorViewWithSave
{
	Q_OBJECT
public:
	/// Constructor
	KetekDetailedDetectorView(IDEASKETEKDetector *detector, QWidget *parent = 0);

	/// Destructor
	virtual ~KetekDetailedDetectorView();

	/// Re-implementing to add the peaking time spin box
	virtual void buildDetectorView();

public slots:
	/// Handles setting the peaking time.
	void onPeakingTimeChanged();
	void onPeakingTimeBoxChanged(const QString &arg1);

protected:
	/// Pointer to actual ketek detector for setting peaking time
	IDEASKETEKDetector *ketekDetector_;

	/// Spin box for peaking time.
	QComboBox *peakingTimeBox_;


};

#endif // KETEKDETAILEDDETECTORVIEW_H
