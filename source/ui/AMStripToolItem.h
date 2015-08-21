#ifndef AMSTRIPTOOLITEM_H
#define AMSTRIPTOOLITEM_H

#include <QObject>

#include "beamline/AMControl.h"
#include "MPlot/MPlotItem.h"

class AMStripToolItem : public QObject
{
	Q_OBJECT

public:
	/// Constructor. Creates item by control.
	explicit AMStripToolItem(AMControl *control, QObject *parent = 0);
	/// Constructor. Creates item by name.
	AMStripToolItem(const QString &pvName, QObject *parent);
	/// Destructor.
	virtual ~AMStripToolItem();

	/// Returns true if this item was created by control.
	bool createdByControl() const { return createdByControl_; }
	/// Returns true if this item was created by name.
	bool createdByName() const { return createdByName_; }
	/// Returns the control.
	AMControl* control() const { return control_; }
	/// Returns the plot item.
	MPlotItem* plotItem() const { return plotItem_; }

signals:
	/// Notifier that the control has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Sets the control.
	void setControl(AMControl *newControl);
	/// Sets the control, by name.
	void setControl(const QString &pvName);

protected:
	/// Initializes the item.
	void initialize();

	/// Creates and returns a control with the given pv name.
	AMControl* createControl(const QString &pvName);
	/// Creates and returns a series suitable for plotting pv updates with accompanying timestamps for the given control.
	MPlotItem* createPlotItem(AMControl *control);

protected:
	/// Bool indicating whether this item was created by control.
	bool createdByControl_;
	/// Bool indicating whether this item was created by name.
	bool createdByName_;
	/// The item's control, the source of values tracked.
	AMControl *control_;
	/// The item's plot representation.
	MPlotItem *plotItem_;
};

#endif // AMSTRIPTOOLITEM_H
