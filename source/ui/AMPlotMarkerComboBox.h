#ifndef AMPLOTMARKERCOMBOBOX_H
#define AMPLOTMARKERCOMBOBOX_H

#include <QComboBox>
#include <QItemDelegate>
#include <QStylePainter>
#include "MPlot/MPlotMarker.h"

class AMPlotMarkerComboBox : public QComboBox
{
	Q_OBJECT
public:
	/// The constructor.
	explicit AMPlotMarkerComboBox(QWidget *parent = 0);
	/// Alternate constructor, allows the available marker options to be set.
	AMPlotMarkerComboBox(const QList<MPlotMarkerShape::Shape> &shapeOptions, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMPlotMarkerComboBox();

	/// Returns the list of shape options.
	QList<MPlotMarkerShape::Shape> shapeOptions() const { return shapeOptions_; }
	/// Returns the currently selected marker shape.
	MPlotMarkerShape::Shape selectedMarkerShape() const;

signals:
	/// Notifier that the current marker shape has changed.
	void currentMarkerShapeChanged(MPlotMarkerShape::Shape newShape);

public slots:
	/// Sets the currently selected marker shape.
	void setSelectedMarkerShape(MPlotMarkerShape::Shape shape);
	/// Adds a marker shape to the list of displayed marker shape options.
	virtual void addItem(const MPlotMarkerShape::Shape &newShape);
	/// Adds many marker shapes to the list of displayed marker shape options.
	virtual void addItems(const QList<MPlotMarkerShape::Shape> &newShapes);
	/// Clears the combo box.
	virtual void clear();

protected slots:
	/// Emits notifier that the currently selected marker shape has changed, with the new shape as the argument.
	void onCurrentIndexChanged();

protected:
	/// Adds all the possible plot marker options to the list of available options.
	void addAllPlotMarkers();

protected:
	/// The list of marker shape options.
	QList<MPlotMarkerShape::Shape> shapeOptions_;
};

#endif // AMPLOTMARKERCOMBOBOX_H
