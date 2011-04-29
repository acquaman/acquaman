#ifndef ROIHELPER_H
#define ROIHELPER_H

#include <QObject>

/// This is a helper class meant to pass on the information of the ROIs to the sum and corrected sum for the four element vortex detector.  It will be used until I make my own XRF mapping software.
class ROIHelper : public QObject
{
	Q_OBJECT
public:
	explicit ROIHelper(QObject *parent = 0);

signals:

public slots:

};

#endif // ROIHELPER_H
