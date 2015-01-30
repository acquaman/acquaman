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
	explicit AMPlotMarkerComboBox(QWidget *parent = 0);

	MPlotMarkerShape::Shape selectedMarkerShape() const;
signals:
	void currentMarkerShapeChanged(MPlotMarkerShape::Shape newShape);
public slots:
protected slots:
	void onCurrentIndexChanged(int);
	
};

#endif // AMPLOTMARKERCOMBOBOX_H
