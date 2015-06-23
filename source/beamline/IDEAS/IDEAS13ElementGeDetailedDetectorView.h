#ifndef IDEAS13ELEMENTGEDETAILEDDETECTORVIEW_H
#define IDEAS13ELEMENTGEDETAILEDDETECTORVIEW_H

#include "ui/IDEAS/IDEASXRFDetailedDetectorViewWithSave.h"
#include "beamline/IDEAS/IDEAS13ElementGeDetector.h"

class IDEAS13ElementGeDetailedDetectorView : public IDEASXRFDetailedDetectorViewWithSave
{
	Q_OBJECT
public:
	/// Constructor
	IDEAS13ElementGeDetailedDetectorView(IDEAS13ElementGeDetector *detector, QWidget *parent = 0);

	/// Destructor
	virtual ~IDEAS13ElementGeDetailedDetectorView();

	/// Re-implementing to add peaking time spin box
	virtual void buildDetectorView();

public slots:
	/// Handles setting the combo box based on peaking value in detector
	void onPeakingTimeChanged();
	/// Handles change in combo box peaking selection, sent to detector
	void onPeakingTimeBoxChanged(const QString &arg1);

protected:
	/// Pointer to actual ketek detector for setting peaking time
	IDEAS13ElementGeDetector *ge13ElementDetector_;

	/// Spin box for peaking time.
	QComboBox *peakingTimeBox_;

};

#endif // G13ELEMENTDETAILEDDETECTORVIEW_H
