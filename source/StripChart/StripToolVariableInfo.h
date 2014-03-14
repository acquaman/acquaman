#ifndef STRIPTOOLVARIABLEINFO_H
#define STRIPTOOLVARIABLEINFO_H

#include <QObject>
#include <QTime>
#include <QColor>

#include "dataman/datasource/AMDataSource.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

class StripToolVariableInfo : public QObject
{
    Q_OBJECT
public:
    explicit StripToolVariableInfo(QObject *parent = 0);

signals:
    void nameChanged(const QString &newName);
    void dataSourceChanged();
    void descriptionChanged(const QString &newDescription);
    void unitsChanged(const QString &newUnits);
    void granularityChanged(const int granularity);
    void colorChanged(const QString &colorName);
    void selectionStateChanged(const bool selected);
    void checkStateChanged(const Qt::CheckState checked);

    void infoChanged();

    void dataSourceValueUpdate(QTime newTime, double newValue);


public:
    bool isValid() const;
    bool hasName() const;
    bool hasDataSource() const;
    bool hasCreationDateTime() const;
    bool hasDescription() const;
    bool hasUnits() const;
    bool hasGranularity() const;
    bool hasColor() const;
    bool hasSelectionState() const;
    bool hasCheckState() const;

    QString name() const;
    AMDataSource* dataSource() const;
    QString description() const;
    QString creationDateTime() const;
    QString units() const;
    int granularity() const;
    QColor color() const;
    QString colorName() const;
    bool selectionState() const;
    Qt::CheckState checkState() const;

public slots:
    void setName(const QString &sourceName);
    void setDataSource(AM0DProcessVariableDataSource* newSource);
    void setDescription(const QString &newDescription);
    void setCreationDateTime(const QString &newCreationInfo);
    void setUnits(const QString &newUnits);
    void setGranularity(const int newGranularity);
    void setColor(const QColor &newColor);
    void setSelectionState(const bool isSelected);
    void setCheckState(Qt::CheckState checked);

protected slots:
    void onDataSourceValueUpdate(const AMnDIndex &start, const AMnDIndex &end);

private:
    void defaultSettings();

private:
    QString name_;
    AM0DProcessVariableDataSource* dataSource_;
    QString description_;
    QString creationDateTime_;
    QString units_;
    int granularity_;
    QColor color_;
    bool selectionState_;
    Qt::CheckState checkState_;


    bool hasName_;
    bool hasDataSource_;
    bool hasDescription_;
    bool hasCreationDateTime_;
    bool hasUnits_;
    bool hasGranularity_;
    bool hasColor_;
    bool hasSelectionState_;
    bool hasCheckState_;
    bool hasTimeUnits_;

};

#endif // STRIPTOOLVARIABLEINFO_H
