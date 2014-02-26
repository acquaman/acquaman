#ifndef STRIPTOOL0DVARIABLEINFO_H
#define STRIPTOOL0DVARIABLEINFO_H

#include <QObject>

#include "StripChart/StripToolBasicVariableInfo.h"

class StripTool0DVariableInfo : public StripToolBasicVariableInfo
{
    Q_OBJECT

    friend class StripToolPV;
    friend class StripTool0DVariable;

public:
    explicit StripTool0DVariableInfo(QObject *parent = 0);

signals:
    void infoChanged();

protected:
    QString timeUnits() const;
    int granularity() const;
    QString colorName() const;

private:
    void setTimeUnits(const QString &newUnits);
    void setGranularity(int newGranularity);
    void setColorName(const QString &newColor);

private:
    QString timeUnits_;
    int granularity_;
    QString colorName_;

};

#endif // STRIPTOOL0DVARIABLEINFO_H
