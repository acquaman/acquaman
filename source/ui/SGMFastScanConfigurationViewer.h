#ifndef SGMFASTSCANCONFIGURATIONVIEWER_H
#define SGMFASTSCANCONFIGURATIONVIEWER_H

#include <QGridLayout>
#include <QComboBox>

#include "acquaman/SGM/SGMFastScanConfiguration.h"

class SGMFastScanConfigurationViewer : public QWidget
{
	Q_OBJECT
public:
    SGMFastScanConfigurationViewer(SGMFastScanConfiguration *sfsc, QWidget *parent = 0);

protected:
    AMScanConfiguration *cfg_;

    QComboBox *availableElements_;
    QGridLayout gl_;
};

#endif // SGMFASTSCANCONFIGURATIONVIEWER_H
