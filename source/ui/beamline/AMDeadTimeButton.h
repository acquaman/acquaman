#ifndef AMDEADTIMEBUTTON_H
#define AMDEADTIMEBUTTON_H

#include <QToolButton>

#include "dataman/datasource/AMDataSource.h"

/// This class takes a data source (expected to be the dead time) and changes the color based on the current value and the chosen reference positions.  The expected behaviour is anything below the "good" setpoint is green, anything above the "bad" setpoint is red, and everything else is yellow.
class AMDeadTimeButton : public QToolButton
{
	Q_OBJECT

public:
	/// Constructor.  Takes two data sources (the data source is assumed to have rank 0), one for the input counts and one for the output counts, the good reference point, and the bad reference point.
	AMDeadTimeButton(AMDataSource *inputCountSource, AMDataSource *outputCountSource, double goodReferencePoint, double badReferencePoint, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMDeadTimeButton();

	/// Returns the data source that this button is listening to.
	AMDataSource *deadTimeSource() const { return inputCountSource_; }
	/// Returns the good reference point currently used by this button.
	double goodReferencePoint() const { return goodReferencePoint_; }
	/// Returns the bad reference point currently used by this button.
	double badReferencecPoint() const { return badReferencecPoint_; }

public slots:
	/// Sets a new data source for the dead time (the data source is assumed to have rank 0).
	void setDeadTimeSources(AMDataSource *inputCountSource, AMDataSource *outputCountSource);
	/// Sets the good reference point.
	void setGoodReferencePoint(double newReference);
	/// Sets the bad reference point.
	void setBadReferencePoint(double newReference);

protected slots:
	/// Method that updates the status text of the button and class update.
	void onDeadTimeUpdated();

protected:
	/// Re-implemented paint event.
	void paintEvent(QPaintEvent *e);

	/// The data source that holds the input counts.
	AMDataSource *inputCountSource_;
	/// The data source that holds the output counts.
	AMDataSource *outputCountSource_;
	/// The good reference point.
	double goodReferencePoint_;
	/// The bad reference point.
	double badReferencecPoint_;
};

#endif // AMDEADTIMEBUTTON_H
