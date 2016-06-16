#ifndef BIOXASGENERICSTEPSCANCONFIGURATIONAXESVIEW_H
#define BIOXASGENERICSTEPSCANCONFIGURATIONAXESVIEW_H

#include <QWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>

#include "acquaman/AMGenericStepScanConfiguration.h"
#include "ui/BioXAS/BioXASGenericStepScanConfigurationAxisView.h"

class BioXASGenericStepScanConfigurationAxesView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit BioXASGenericStepScanConfigurationAxesView(AMGenericStepScanConfiguration *configuration, BioXASScanAxisRegionView::ViewMode viewMode = BioXASScanAxisRegionView::Absolute, AMControlSet *controls = 0, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASGenericStepScanConfigurationAxesView();

	/// Returns the view mode.
	BioXASScanAxisRegionView::ViewMode viewMode() const { return viewMode_; }
	/// Returns the configuration being viewed.
	AMGenericStepScanConfiguration* configuration() const { return configuration_; }
	/// Returns the axis control options.
	AMControlSet* controls() const { return controls_; }

signals:
	/// Notifier that the view mode has changed.
	void viewModeChanged(BioXASScanAxisRegionView::ViewMode newMode);
	/// Notifier that the configuration being viewed has changed.
	void configurationChanged(AMGenericStepScanConfiguration *newConfiguration);
	/// Notifier that the axis control options have changed.
	void controlsChanged(AMControlSet *newControls);

public slots:
	/// Sets the view mode.
	void setViewMode(BioXASScanAxisRegionView::ViewMode newMode);
	/// Sets the configuration being viewed.
	void setConfiguration(AMGenericStepScanConfiguration *newConfiguration);
	/// Sets the axis control options.
	void setControls(AMControlSet *newControls);

protected slots:
	/// Updates the axis views.
	void updateAxisViews();
	/// Updates the absolute view mode button.
	void updateAbsoluteButton();
	/// Updates the relative view mode button.
	void updateRelativeButton();

	/// Handles updating the view mode in response to changes in the absolute or relative buttons.
	void onViewModeButtonsClicked();

protected:
	/// Returns a newly created axis view.
	virtual QWidget* createAxisView(AMGenericStepScanConfiguration *configuration, int axisNumber, BioXASScanAxisRegionView::ViewMode viewMode, AMControlSet *controls) const;

protected:
	/// The view mode.
	BioXASScanAxisRegionView::ViewMode viewMode_;
	/// The configuration being viewed.
	AMGenericStepScanConfiguration *configuration_;
	/// The axis control options.
	AMControlSet *controls_;

	/// The axis views layout.
	QVBoxLayout *axisViewsLayout_;
	/// The list of axis views.
	QList<QWidget*> axisViews_;

	/// The Absolute view mode radio button.
	QRadioButton *absoluteButton_;
	/// The Relative view mode radio button.
	QRadioButton *relativeButton_;
};

#endif // BIOXASGENERICSTEPSCANCONFIGURATIONAXESVIEW_H
