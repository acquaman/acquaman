#ifndef STRIPTOOL_H
#define STRIPTOOL_H

#include <QObject>
#include <QStringListModel>

#include "MPlot/MPlot.h"

#include "beamline/AMPVControl.h"

class StripTool : public QObject
{
    Q_OBJECT

public:
    explicit StripTool(QObject *parent = 0);

    friend class StripToolView;
    
signals:
    void pvAlreadyVisible();

protected:
    QStringList activePVNamesList_;
    QStringListModel *activePVNames_;

    QStringList activePVDescriptionsList_;
    QStringListModel *activePVDescriptions_;

    QStringList visiblePVNamesList_;
    QStringListModel *visiblePVNames_;

protected:
    void addToActivePVs(const QString &pvName, const QString &pvDescription);
    void removeFromActivePVs(const QString &pvName);
    void addToVisiblePVs(const QString &pvName);
    void removeFromVisiblePVs(const QString &pvName);
    QStringListModel* getActivePVNames();
//    QStringListModel getActivePVDescriptions();


public slots:
    
};

#endif // STRIPTOOL_H
