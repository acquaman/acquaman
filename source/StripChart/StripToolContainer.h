#ifndef STRIPTOOLCONTAINER_H
#define STRIPTOOLCONTAINER_H

#include <QObject>

#include "StripChart/StripToolItem.h"

class StripToolContainer : public QObject
{
    Q_OBJECT

public:
    explicit StripToolContainer(QObject *parent = 0);
    friend class StripTool;
    
signals:
    void pvDataUpdate(const QString &pvName, QVector<double> xValues, QVector<double> yValues);

protected:
    int itemCount_;
//    QList<StripToolItem *> items_;
    QMap<QString, StripToolItem *> pvNameToItemMap;

protected:
    void addItem(const QString &pvName, const QString &pvDescription);
    void removeItem(const QString &pvName);
    bool contains(const QString &pvName);
    void setValuesDisplayed(const QString &pvName, const int newValuesDisplayed);
    
public slots:
    void onPVDataUpdate(const QString &pvName, QVector<double> xValues, QVector<double> yValues);
    
};

#endif // STRIPTOOLCONTAINER_H
