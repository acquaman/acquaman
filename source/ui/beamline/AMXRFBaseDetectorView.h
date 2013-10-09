#ifndef AMXRFBASEDETECTORVIEW_H
#define AMXRFBASEDETECTORVIEW_H

#include <QWidget>

#include "beamline/AMXRFDetector.h"

#include "MPlot/MPlot.h"
#include "MPlot/MPlotWidget.h"
#include "MPlot/MPlotSeries.h"
#include "ui/AMTopFrame.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>

/// This class provides a very basic view for any AMXRFDetector.  It provides very basic control and feedback with a spectrum viewer.
class AMXRFBaseDetectorView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes in an AMXRFDetector pointer.
	explicit AMXRFBaseDetectorView(AMXRFDetector *detector, QWidget *parent = 0);

	/// Builds the for the detector and must be called after the view has been constructed.  All building of the GUI should be done inside of this method.  Reimplement this method when you want to customize the entire look and feel of the detector view.
	virtual void buildDetectorView();

	/// Sets the title bar.  Sets the name and icon for the bar.
	void setTitleBar(const QString &title, const QIcon &icon);
	/// Sets the title bar text.
	void setTitleBarText(const QString &title);
	/// Sets the title bar icon.
	void setTitleBarIcon(const QIcon &icon);
	/// Sets the visibility of the tital bar.  Default to visible.
	void setTitleBarVisibility(bool isVisible);
	/// Returns the title bar text.
	const QString title() const { return topFrame_->title(); }
	/// Returns the title bar visibility.
	bool titleBarIsVisible() const { return topFrame_->isVisible(); }
	/// Returns the title bar icon.
	const QIcon titleIcon() const { return topFrame_->icon(); }

signals:

public slots:

protected slots:
	/// Updates the status label when the detector acquisition state changes.
	void onAcquisitionStateChanged(AMDetector::AcqusitionState state);
	/// Sets the detector acquisition time.
	void onNewAcquisitionTimeSetpoint();
	/// Handles updates to the elapsed time.
	void onElapsedTimeChanged(double time);

protected:
	/// Sets up the plot.  Can add the plot widget to the layout after calling this function.  Can be reimplemented for customizing the plot as desired.  This is called inside of AMXRFBaseDetectorView::buildDetectorView().  If completely re-implementing buildDetectorView, you need to make sure to call this method inside your own buildDetectorView().
	virtual void setupPlot();

	/// The pointer to the detector.
	AMXRFDetector *detector_;

	/// This is the plot widget that holds the plot used for viewing the spectra.
	MPlotWidget *plotView_;
	/// This is the plot itself.
	MPlot *plot_;
	/// A list of plot items that are spectra currently being displayed in the plot.
	QList<MPlotSeriesBasic *> spectraPlotItems_;

	/// The top frame title bar.
	AMTopFrame *topFrame_;

	// The GUI elements for basic AMXRFDetectors.
	/// The acquire button.
	QPushButton *acquireButton_;
	/// The cancle button.
	QPushButton *cancelButton_;
	/// The acquire time spin box.
	QDoubleSpinBox *acquireTimeSpinBox_;
	/// The elapsed time label.
	QLabel *elapsedTimeLabel_;
	/// The status label.
	QLabel *statusLabel_;

	// The main layouts.
	/// The master layout that holds all of the other layouts in the base view.
	QGridLayout *masterLayout_;
	/// The left vertical layout.
	QVBoxLayout *leftLayout_;
	/// The right vertical layout.
	QVBoxLayout *rightLayout_;
	/// The top horizontal layout.
	QHBoxLayout *topLayout_;
	/// The bottom horizontal layout.
	QHBoxLayout *bottomLayout_;
	/// The horizontal layout that holds the plot.
	QHBoxLayout *plotLayout_;
};

#endif // AMXRFBASEDETECTORVIEW_H
