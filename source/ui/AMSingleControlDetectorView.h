#ifndef AMSINGLECONTROLDETECTORVIEW_H
#define AMSINGLECONTROLDETECTORVIEW_H

#include "AMDetectorView.h"

class AMSingleControlBriefDetectorView : public AMBriefDetectorView
{
Q_OBJECT
public:
	Q_INVOKABLE explicit AMSingleControlBriefDetectorView(AMSingleControlDetector *detector = 0, QWidget *parent = 0);

	AMDetector* detector();

protected:
	AMControlEditor *fbk_;
	QHBoxLayout *hl_;
	AMSingleControlDetector *detector_;

	/// We are trusting createDetectorView to pass in the correct type of detector, sub classes should trust AMDetector is actually their type
	bool setDetector(AMDetector *detector, bool configureOnly = false);
};

#endif // AMSINGLECONTROLDETECTORVIEW_H
