#ifndef G13ELEMENTDETAILEDDETECTORVIEW_H
#define G13ELEMENTDETAILEDDETECTORVIEW_H

#include "ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h"
#include "beamline/IDEAS/IDEAS13ElementGeDetector.h"

class g13ElementDetailedDetectorView : public IDEASXRFDetailedDetectorViewWithSave
{
	Q_OBJECT
public:
	/// Constructor
	g13ElementDetailedDetectorView(IDEAS13ElementGeDetector *detector, QWidget *parent = 0);

	/// Destructor
	virtual ~g13ElementDetailedDetectorView();

	/// Re-implementing to add peaking time spin box
	virtual void buildDetectorView();

public slots:
	/// Handles setting the peaking time.
	void onPeakingTimeChanged();
	void onPeakingTimeBoxChanged(const QString &arg1);

protected:
	/// Pointer to actual ketek detector for setting peaking time
	IDEAS13ElementGeDetector *g13ElementDetector_;

	/// Spin box for peaking time.
	QComboBox *peakingTimeBox_;

};

#endif // G13ELEMENTDETAILEDDETECTORVIEW_H
