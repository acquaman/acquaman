#ifndef IDEASKETEKDETAILEDDETECTORVIEW_H
#define IDEASKETEKDETAILEDDETECTORVIEW_H

#include "ui/IDEAS/IDEASXRFDetailedDetectorView.h"
#include "beamline/IDEAS/IDEASKETEKDetector.h"

class IDEASKETEKDetailedDetectorView : public IDEASXRFDetailedDetectorView
{
	Q_OBJECT
public:
	/// Constructor
	IDEASKETEKDetailedDetectorView(IDEASKETEKDetector *detector, QWidget *parent = 0);

	/// Destructor
	virtual ~IDEASKETEKDetailedDetectorView();

	/// Re-implementing to add the peaking time spin box
	virtual void buildDetectorView();

public slots:
	/// Handles setting the combo box based on peaking value in detector
	void onPeakingTimeChanged();
	/// Handles change in combo box peaking selection, sent to detector
	void onPeakingTimeBoxChanged(const QString &arg1);

protected:
	/// Pointer to actual ketek detector for setting peaking time
	IDEASKETEKDetector *ketekDetector_;

	/// Spin box for peaking time.
	QComboBox *peakingTimeBox_;


};

#endif // IDEASKETEKDETAILEDDETECTORVIEW_H
