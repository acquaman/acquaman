#ifndef AMSCANAXISVIEW_H
#define AMSCANAXISVIEW_H

#include <QWidget>

#include "acquaman/AMStepScanConfiguration.h"

/// The scan axis setter view.
class AMScanAxisView : public QWidget
{
public:
	/// Constructor.
	AMScanAxisView(const QString &title, AMStepScanConfiguration *configuration, QWidget *parent = 0);

signals:

public slots:
};

#endif // AMSCANAXISVIEW_H
