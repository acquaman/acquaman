#ifndef SGMFASTSCANCONFIGURATIONVIEWER_H
#define SGMFASTSCANCONFIGURATIONVIEWER_H

#include <QGridLayout>
#include <QComboBox>

#include <algorithm>

#include "AMScanConfigurationViewer.h"
#include "acquaman/SGM/SGMFastScanConfiguration.h"

class SGMFastScanConfigurationViewer : public AMScanConfigurationViewer
{
	Q_OBJECT
public:
    SGMFastScanConfigurationViewer(SGMFastScanConfiguration *sfsc, QWidget *parent = 0);

protected slots:
    void onLockdowScanning(bool isLocked, QString reason);

protected:
    AMScanConfiguration *cfg_;

    QComboBox *availableElements_;
    QPushButton *startScanButton_;
    QPushButton *addToQueueButton_;
    QPushButton *queueDirectorButton_;
    QGridLayout gl_;
};

#endif // SGMFASTSCANCONFIGURATIONVIEWER_H
