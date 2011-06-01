#ifndef VESPERSTEMPERATUREVIEW_H
#define VESPERSTEMPERATUREVIEW_H

#include <QWidget>

class VESPERSTemperatureView : public QWidget
{
	Q_OBJECT
public:
	/// Constructor.  Builds the view based on the temperature control set.
	explicit VESPERSTemperatureView(QWidget *parent = 0);

signals:

public slots:

};

#endif // VESPERSTEMPERATUREVIEW_H
