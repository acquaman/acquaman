#ifndef IDEASXASSCANCONFIGURATIONVIEW_H
#define IDEASXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include "acquaman/IDEAS/IDEASXASScanConfiguration.h"

#include "util/AMElement.h"


#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QLabel>
#include <QToolButton>

class AMScanController;
class AMEXAFSScanAxisView;
class AMTopFrame;

class IDEASXASScanConfigurationView : public AMScanConfigurationView
{
Q_OBJECT

public:
	IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent = 0);
	virtual ~IDEASXASScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const;


protected:
	IDEASXASScanConfiguration *configuration_;

	AMTopFrame *topFrame_;
	AMEXAFSScanAxisView *regionsView_;
        QPushButton *autoRegionButton_;
	QPushButton *pseudoXAFSButton_;

	QCheckBox *isXRFScanCheckBox_;
	QCheckBox *isTransScanCheckBox_;
	QCheckBox *useRefCheckBox_;

//	QComboBox *I0ChannelComboBox_;
//	QComboBox *ItChannelComboBox_;
//	QComboBox *IrChannelComboBox_;

	QLineEdit *scanName_;

	/// Double spin box for changing the energy.
	QDoubleSpinBox *energy_;
	/// Button used to choose an element to scan over.
	QToolButton *elementChoice_;
	/// The combo box that holds all the lines that can be chosen to scan over.
	QComboBox *lineChoice_;
	/// Label holding the current estimated time for the scan to complete.  Takes into account extra time per point based on experience on the beamline.
	QLabel *estimatedTime_;
	/// Label holding the current estimated time for the set of scans to complete.
	QLabel *estimatedSetTime_;




protected slots:
        void onAutoRegionButtonClicked();
	void onXAFSRegionButtonClicked();

	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();
	/// Sets the new energy.
	void setEnergy();
	/// Handles choosing a new element when the element button is clicked.
	void onElementChoiceClicked();
	/// Fills in the combo box with lines that can be scanned.
	void fillLinesComboBox(AMElement *el);
	/// Handles changes in the combo box index.
	void onLinesComboBoxIndexChanged(int index);
	/// Handles setting the proper information if the edge is changed.
	void onEdgeChanged();
	/// Helper slot that handles the setting the estimated time label.
	void onEstimatedTimeChanged();




};

#endif // IDEASXASSCANCONFIGURATIONVIEW_H
