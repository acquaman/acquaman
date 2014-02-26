#ifndef STRIPTOOLPV_H
#define STRIPTOOLPV_H

#include <QObject>
#include <QDebug>

#include "StripToolBasicVariable.h"
#include "beamline/AMPVControl.h"
#include "dataman/datasource/AMProcessVariableDataSource.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"
#include "StripToolSeries.h"
#include "StripTool0DVariableInfo.h"

/// This class keeps track of all information relevant to a given pv: pv name, description, units, check state, value updates.

class StripToolPV : public StripToolBasicVariable
{
    Q_OBJECT

public:
    StripToolPV(StripTool0DVariableInfo *info, AMDataSource *dataSource = 0, QObject *parent = 0);
    ~StripToolPV();
    friend class StripToolModel;
    friend class StripToolDataController;

signals:
    // these are external signals, meant to inform the model of different changes within this particular pv.
    /// Pv signals to the model when it is time to save pv data to file.
    void savePVData();
    /// Pv signals to the model when a change has been made to the pv's metadata.
    void savePVMetaData();
    /// Signals the model that the pv's current value has been updated.
    void pvValueUpdated();

    void dataSourceUpdated(double newValue);
    void dataRangeChanged(MPlotAxisRange *newRange);
    void displayRangeChanged(MPlotAxisRange *newRange);
    void dataMaxChanged(double newMax);
    void dataMinChanged(double newMin);
    void maxTimeBetweenUpdatesChanged(double seconds);

    /// When the update interval timer has counted out the maxTimeBetweenUpdates, the pv will 'manually' report a change in value (a value identical to the last update), even if the control didn't signal that one happened.
    void manuallyUpdatePV(double value);

protected:
    StripTool0DVariableInfo* info() const;

    /// The pvs are all plotted against time, and the units of time are returned here.
    QString xUnits() const;
    /// Returns the checkstate for this pv : checked indicates it should appear on the plot, unchecked that it should be hidden.
    Qt::CheckState checkState();
    /// Returns the color set for this pv's series.
    QColor color();
    int updateGranularity();
    QString customDisplayedYMax();
    QString customDisplayedYMin();
    double displayedYMin();
    double displayedYMax();
    double maxTimeBetweenUpdates();
    /// Returns a pointer to the pv's instance of MPlotVectorSeriesData. This was useful earlier, when a pv's series was actually a property of the plot, but I don't think it's used now.
    MPlotVectorSeriesData* data();
    /// Returns a pointer to the series that should be plotted for this pv.

    StripToolSeries* series() const;
    MPlotItem* plotItem() const;

    int timeDisplayed();
    /// This is here because I thought it would be neat to have the pv data saved periodically, which would be handled by the model. This could cut down on memory needed, if this application runs for a long time (but isn't fully implemented!).
    QVector<QString> saveMasterTimes();
    QVector<double> saveMasterValues();
    bool setMetaData(QList<QString> metaData);
    bool isSelected();
    void setSelected(bool selected);
    void setMaxTimeBetweenUpdates(double seconds);
    /// The user can pause and restart the value updates displayed on the plot (this class will still continue to record updates).
    void setPVUpdating(bool isUpdating);
    /// The list view will allow the user to un/check a pv, and the most recent update for a pv's check state is set here.
//    void setCheckState(Qt::CheckState isChecked);
    void setDisplayedYMax(const QString &newMax);
    void setDisplayedYMin(const QString &newMin);

private:
    void setGranularity(const QString &newVal);
    void setGranularity(int newVal);


    void setXUnits(const QString &newUnits);
    void setTimeDisplayed(int seconds);
//    void setMetaDataHeaders();
    void saveCheck();
    void dataVectorSizeCheck();
    void toUpdateValues(double);

    void setColor(const QString &colorName);
//    void setCheckState(Qt::CheckState isChecked);


protected slots:
    void onInfoChanged();
    void onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end);
    void toUpdateTime(int newTime);
    void toUpdateTimeUnits(const QString &newUnits);
    void toManuallyUpdatePV();

private:
    StripTool0DVariableInfo* info_;

    int updateIndex_;
    int updateGranularity_;

    int defaultTimeDisplayed_;
    int timeDisplayed_;

    int dataVectorSize_;
    int savePoint_;

    QString xUnits_;
    bool isUpdating_;
    Qt::CheckState checkState_;
    QColor color_;

    QTimer *updateIntervalTimer_;
    int maxTimeBetweenUpdates_;

    bool isSelected_;

    double timeFactor_;

    QVector<QTime> masterUpdateTimes_;
    QVector<double> masterUpdateValues_;

    QVector<double> displayedTimes_;
    QVector<double> displayedValues_;

    MPlotVectorSeriesData *pvData_;
    StripToolSeries *pvSeries_;

};

#endif // STRIPTOOLPV_H
