#include "StripTool.h"

StripTool::StripTool(QObject *parent) :
    QObject(parent)
{
    //QStringList activePVNamesList_;

    activePVNames_ = new QStringListModel(this);
    activePVNames_->setStringList(activePVNamesList_);

    activePVDescriptions_ = new QStringListModel(this);
    activePVDescriptions_->setStringList(activePVDescriptionsList_);

    visiblePVNames_ = new QStringListModel(this);
    visiblePVNames_->setStringList(visiblePVNamesList_);
}

void StripTool::addToActivePVs(const QString &pvName, const QString &pvDescription)
{
    if (activePVNamesList_.contains(pvName))
    {
        if (visiblePVNamesList_.contains(pvName))
            emit pvAlreadyVisible();
        else
            addToVisiblePVs(pvName);

    } else {
        activePVNamesList_.append(pvName);
        activePVDescriptionsList_.append(pvDescription);
        addToVisiblePVs(pvName);
    }
}

void StripTool::removeFromActivePVs(const QString &pvName)
{
    int pvIndex = activePVNamesList_.indexOf(pvName);
    activePVNamesList_.removeAt(pvIndex);
    activePVDescriptionsList_.removeAt(pvIndex);

    removeFromVisiblePVs(pvName);
}

void StripTool::addToVisiblePVs(const QString &pvName)
{
    visiblePVNamesList_.append(pvName);
}

void StripTool::removeFromVisiblePVs(const QString &pvName)
{
    visiblePVNamesList_.removeAt(visiblePVNamesList_.indexOf(pvName));
}

QStringListModel* StripTool::getActivePVNames()
{
    return activePVNames_;
}

//QStringListModel StripTool::getActivePVDescriptions()
//{
//    return activePVDescriptionsList_;
//}
