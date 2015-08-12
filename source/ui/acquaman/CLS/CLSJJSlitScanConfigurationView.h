#ifndef CLSJJSLITSCANCONFIGURATIONVIEW_H
#define CLSJJSLITSCANCONFIGURATIONVIEW_H

#include "acquaman/CLS/CLSJJSlitScanConfiguration.h"
#include "ui/acquaman/AMScanConfigurationView.h"

#include <QRadioButton>
#include <QGroupBox>
#include <QLayout>
#include <QScrollArea>
#include <QCheckBox>
#include <QButtonGroup>

class CLSJJSlitScanConfigurationView : public AMScanConfigurationView
{
    Q_OBJECT

public:
	/// Constructor.
	explicit CLSJJSlitScanConfigurationView(CLSJJSlitScanConfiguration *configuration, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitScanConfigurationView();

	/// Returns the configuration being viewed.
	virtual CLSJJSlitScanConfiguration* configuration() const { return configuration_; }

protected slots:
	/// Updates the configuration with the desired slit direction, in response to a change in the direction options.
	void onDirectionSelectionChanged(QAbstractButton *button);
	/// Updates the configuration with the desired slit property, in response to a change in the property options.
	void onPropertySelectionChanged(QAbstractButton *button);
	/// Updates the configuration detector infos, in response to a change in the detector selections.
	void onDetectorSelectionChanged(QAbstractButton *button);

	/// Updates the scan configuration controls.
	void updateScanConfigurationControls(CLSJJSlits::Direction::Option direction, CLSJJSlits::Property::Option property);

protected:
	/// The configuration being viewed.
	CLSJJSlitScanConfiguration *configuration_;

	/// The currently selected slit direction.
	CLSJJSlits::Direction::Option direction_;
	/// The currently selected slit property.
	CLSJJSlits::Property::Option property_;
};

#endif // CLSJJSLITSCANCONFIGURATIONVIEW_H
