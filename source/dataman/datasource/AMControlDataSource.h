#ifndef AMCONTROLDATASOURCE_H
#define AMCONTROLDATASOURCE_H

#include <QObject>

#include "dataman/datasource/AMProcessVariableDataSource.h"
#include "beamline/AMControl.h"

class AMControlDataSource : public QObject, public AMDataSource
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMControlDataSource(AMControl *control, const QString &name, QObject *parent = 0);
	/// Destructor.
	virtual ~AMControlDataSource();

	/// Returns the control.
	AMControl* control() const { return control_; }

	/// Returns a description.
	virtual QString typeDescription() const;
	/// Returns an OR-combination of StateFlags describing the current state of the data.
	virtual int state() const;
	/// Returns axis information for all axes.
	virtual QList<AMAxisInfo> axes() const;

	/// Returns the rank of this data set.
	virtual int rank() const;
	/// Returns the size of each dimension.
	virtual AMnDIndex size() const;
	/// Returns the size along a single axis.
	virtual int size(int axisNumber) const;
	/// Returns information about a particular axis.
	virtual AMAxisInfo axisInfoAt(int axisNumber) const;
	/// Returns the id of an axis, by name.
	virtual int idOfAxis(const QString &axisName) const;

	/// Returns the dependent value at a complete set of axis indexes.
	virtual AMNumber value(const AMnDIndex &indexes) const;
	/// Returns the independent values along an axis.
	virtual AMNumber axisValue(int axisNumber, int index) const;

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *control);

protected slots:
	/// Sets the control.
	void setControl(AMControl *newControl);

	/// Emits the data changed signal when the control gets new data.
	void onDataChanged();
	/// Emits the state changed signal when the control's state changes.
	void onStateChanged();

protected:
	/// The control used as the source of data updates.
	AMControl *control_;
	/// A list axis infos.
	QList<AMAxisInfo> axes_;

};

#endif // AMCONTROLDATASOURCE_H
