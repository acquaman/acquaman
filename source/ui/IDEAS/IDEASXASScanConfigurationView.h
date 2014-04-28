#ifndef IDEASXASSCANCONFIGURATIONVIEW_H
#define IDEASXASSCANCONFIGURATIONVIEW_H

#include "ui/acquaman/AMScanConfigurationView.h"

#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

class AMScanController;
class IDEASXASScanConfiguration;
class AMEXAFSScanAxisView;
class AMTopFrame;

class IDEASXASScanConfigurationView : public AMScanConfigurationView
{
Q_OBJECT

public:
	IDEASXASScanConfigurationView(IDEASXASScanConfiguration *configuration, QWidget *parent = 0);
	virtual ~IDEASXASScanConfigurationView();

	virtual const AMScanConfiguration* configuration() const;

protected slots:
	/// Handles setting the name of the configuration from the line edit.
	void onScanNameEdited();

protected:
	IDEASXASScanConfiguration *configuration_;

	AMTopFrame *topFrame_;
	AMEXAFSScanAxisView *regionsView_;
        QPushButton *autoRegionButton_;
	QPushButton *pseudoXAFSButton_;

	QCheckBox *isXRFScanCheckBox_;
	QCheckBox *isTransScanCheckBox_;
	QComboBox *I0ChannelComboBox_;
	QComboBox *ItChannelComboBox_;
	QComboBox *IrChannelComboBox_;

	QLineEdit *scanName_;
	QDoubleSpinBox *edgeEnergy_;



protected slots:
        void onAutoRegionButtonClicked();
	void onXAFSRegionButtonClicked();
};

#endif // IDEASXASSCANCONFIGURATIONVIEW_H
