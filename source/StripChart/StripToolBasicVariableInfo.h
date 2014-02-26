#ifndef STRIPTOOLBASICVARIABLEINFO_H
#define STRIPTOOLBASICVARIABLEINFO_H

#include <QObject>
#include <QDateTime>

#include "StripChart/StripToolAbstractVariableInfo.h"

class StripToolBasicVariableInfo : public StripToolAbstractVariableInfo
{
    Q_OBJECT

    friend class StripToolBasicVariable;

public:
    explicit StripToolBasicVariableInfo(QObject *parent = 0);

signals:
    void infoChanged();

protected:
    QString creationDateTime() const;
    QString description() const;
    QString units() const;
    Qt::CheckState checkState() const;
    bool selectionState() const;

private:
    void setCreationDateTime(const QString &newCreationInfo);
    void setDescription(const QString &newDescription);
    void setUnits(const QString &newUnits);
    void setCheckState(Qt::CheckState isChecked);
    void setSelectionState(bool isSelected);

private:    
    QString creationDateTime_;
    QString description_;
    QString units_;
    Qt::CheckState checkState_;
    bool selectionState_;

};

#endif // STRIPTOOLBASICVARIABLEINFO_H
