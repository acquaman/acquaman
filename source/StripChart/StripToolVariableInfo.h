#ifndef STRIPTOOLVARIABLEINFO_H
#define STRIPTOOLVARIABLEINFO_H

#include <QObject>
#include <QTime>
#include <QColor>

#include "dataman/datasource/AMDataSource.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"

#include "StripChart/TimeEntryWidget.h"

class TimeEntryWidget;

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
    void dataMaxChanged(double newMax);
    void dataMinChanged(double newMin);
    void axisMaxChanged(double newMax);
    void axisMinChanged(double newMin);
    void axisMaxErased();
    void axisMinErased();

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
    bool hasAverageOverPoints() const;
    bool hasColor() const;
    bool hasSelectionState() const;
    bool hasCheckState() const;
    bool hasCustomAxisMax() const;
    bool hasCustomAxisMin() const;

    QString name() const;
    AMDataSource* dataSource() const;
    QString description() const;
    QString creationDateTime() const;
    QString units() const;
    int granularity() const;
    int averageOverPoints() const;
    QColor color() const;
    QString colorName() const;
    bool selectionState() const;
    Qt::CheckState checkState() const;
    double customAxisMax() const;
    double customAxisMin() const;

    int timeAmount() const;
    TimeEntryWidget::TimeUnits timeUnits() const;
    int displayedTimeMS() const;
    double timeMSToTimeUnits(int measurementTimeMS) const;
    bool importAutomatically() const;

public slots:
    void setName(const QString &sourceName);
    void setDataSource(AMDataSource* newSource);
    void setDescription(const QString &newDescription);
    void setCreationDateTime(const QString &newCreationInfo);
    void setUnits(const QString &newUnits);
    void setGranularity(const int newGranularity);
    void setAverageOverPoints(const int avgPoints);
    void setColor(const QColor &newColor);
    void setSelectionState(const bool isSelected);
    void setCheckState(Qt::CheckState checked);
    void setCustomAxisMax(const double newMax);
    void setCustomAxisMin(const double newMin);
    void eraseCustomAxisMax();
    void eraseCustomAxisMin();
    void setTimeAmount(int newTime);
    void setTimeUnits(TimeEntryWidget::TimeUnits units);
    void setToImportAutomatically(const bool import);

protected slots:
    void onDataSourceValueUpdate(const AMnDIndex &start, const AMnDIndex &end);

private:
    void defaultSettings();

private:
    QString name_;
    AMDataSource* dataSource_;
    QString description_;
    QString creationDateTime_;
    QString units_;
    int granularity_;
    int averageOverPoints_;
    QColor color_;
    bool selectionState_;
    Qt::CheckState checkState_;

    double customAxisMax_;
    double customAxisMin_;

    int timeAmount_;
    TimeEntryWidget::TimeUnits timeUnits_;

    bool importAutomatically_;

    bool hasName_;
    bool hasDataSource_;
    bool hasDescription_;
    bool hasCreationDateTime_;
    bool hasUnits_;
    bool hasGranularity_;
    bool hasAverageOverPoints_;
    bool hasColor_;
    bool hasSelectionState_;
    bool hasCheckState_;
    bool hasCustomAxisMax_;
    bool hasCustomAxisMin_;
};

#endif // STRIPTOOLVARIABLEINFO_H
