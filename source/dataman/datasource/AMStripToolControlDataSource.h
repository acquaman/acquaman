#ifndef AMSTRIPTOOLCONTROLDATASOURCE_H
#define AMSTRIPTOOLCONTROLDATASOURCE_H

#include "analysis/AM0DAccumulatorAB.h"
#include "analysis/AM0DTimestampAB.h"
#include "analysis/AM1DTimedDataAB.h"

#include "beamline/AMControl.h"

#include "dataman/datasource/AMControlDataSource.h"

class AMStripToolControlDataSource : public AM1DTimedDataAB
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMStripToolControlDataSource(AMControl *control, const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMStripToolControlDataSource();

	/// Returns the control.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *control);

protected slots:
	/// Sets the control.
	void setControl(AMControl *newControl);

protected:
	/// Creates and returns a data source containing control value updates.
	AM0DAccumulatorAB* createValuesDataSource(AMControlDataSource *controlSource);
	/// Creates and returns a data source containing control value update timestamps.
	AM0DTimestampAB* createTimesDataSource(AMControlDataSource *controlSource);

protected:
	/// The control used as the source of data updates.
	AMControl *control_;

	/// The control data source.
	AMControlDataSource *controlSource_;
	/// The collection of control value updates.
	AM0DAccumulatorAB *valuesSource_;
	/// The collection of control value update timestamps.
	AM0DTimestampAB *timesSource_;
};

#endif // AMSTRIPTOOLCONTROLDATASOURCE_H
