#ifndef STRIPTOOLPV_H
#define STRIPTOOLPV_H

#include <QObject>
#include <QDebug>

#include "beamline/AMPVControl.h"
#include "MPlot/MPlotSeriesData.h"
#include "MPlot/MPlotSeries.h"

/// This class keeps track of all information relevant to a given pv: pv name, description, units, check state, value updates.

class StripToolPV : public QObject
{
    Q_OBJECT

public:
    StripToolPV(QObject *parent = 0);
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
    ///
    void descriptionChanged(const QString &newDescription);
    void unitsChanged(const QString &units);
    void dataRangeChanged(MPlotAxisRange *newRange);
    void dataMaxChanged(double newMax);
    void dataMinChanged(double newMin);
    void waterfallChanged(double shiftAmount);
    void maxTimeBetweenUpdatesChanged(double seconds);

    // these are internal signals, used to trigger other internal events in response to something happening.
    /// When the control's latest value updates, the first step to restarting the updating interval timer is to stop it!
    void stopUpdateIntervalTimer();
    /// When the update interval timer has counted out the maxTimeBetweenUpdates, the pv will 'manually' report a change in value (a value identical to the last update), even if the control didn't signal that one happened.
    void manuallyUpdatePV(double value);

protected:
    int updateIndex_;
    int updateGranularity_;

    int defaultTimeDisplayed_;
    int timeDisplayed_;

    int dataVectorSize_;
    int savePoint_;

    QList<QString> headers_;

    QString pvName_;
    QString dateCreated_;
    QString pvDescription_;
    QString xUnits_;
    QString yUnits_;
    bool isUpdating_;
    Qt::CheckState checkState_;
    QColor pvColor_;

    QTimer *updateIntervalTimer_;
    int maxTimeBetweenUpdates_;

    bool isSelected_;

    double timeFactor_;

    QVector<QTime> masterUpdateTimes_;
    QVector<double> masterUpdateValues_;

    QVector<double> displayedTimes_;
    QVector<double> displayedValues_;

    double defaultDisplayedYMin_;
    double defaultDisplayedYMax_;

    QString customDisplayedYMin_;
    QString customDisplayedYMax_;

    bool defaultYMaxDisplayed_;
    bool defaultYMinDisplayed_;

    MPlotVectorSeriesData *pvData_;
    MPlotSeriesBasic *pvSeries_;
    AMControl *pvControl_;

protected:
    /// Returns the epics pv name for this pv.
    QString pvName() const;
    /// Returns the description for this pv, if the user has provided one.
    QString pvDescription() const;
    /// The pvs are all plotted against time, and the units of time are returned here.
    QString xUnits() const;
    /// Returns the pv's units, if the user has provided them.
    QString yUnits() const;
    /// Returns the checkstate for this pv : checked indicates it should appear on the plot, unchecked that it should be hidden.
    Qt::CheckState checkState();
    /// Returns the color set for this pv's series.
    QColor color();
    int updateGranularity();
    double displayedYMin();
    double displayedYMax();
    double maxTimeBetweenUpdates();
    /// Returns a pointer to the pv's instance of MPlotVectorSeriesData. This was useful earlier, when a pv's series was actually a property of the plot, but I don't think it's used now.
    MPlotVectorSeriesData* data();
    /// Returns a pointer to the series that should be plotted for this pv.
    MPlotSeriesBasic* series();
    int timeDisplayed();
    /// This is here because I thought it would be neat to have the pv data saved periodically, which would be handled by the model. This could cut down on memory needed, if this application runs for a long time (but isn't fully implemented!).
    QVector<QString> saveMasterTimes();
    QVector<double> saveMasterValues();
    QList<QString> metaDataHeaders();
    QList<QString> metaData();
    QList<QString> editPVDialogData();
    bool setMetaData(QList<QString> metaData);
    bool isSelected();
    void setSelected(bool selected);
    /// Sets the PV control and reparents for this pv.
    void setControl(AMControl *newControl);
    void setMaxTimeBetweenUpdates(double seconds);
    /// The user can pause and restart the value updates displayed on the plot (this class will still continue to record updates).
    void setPVUpdating(bool isUpdating);
    /// The list view will allow the user to un/check a pv, and the most recent update for a pv's check state is set here.
    void setCheckState(Qt::CheckState isChecked);
    void setDisplayedYMax(const QString &newMax);
    void setDisplayedYMin(const QString &newMin);
    bool operator== (const StripToolPV &anotherPV);

private:
    void setDescription(const QString &newDescription);
    void setYUnits(const QString &newUnits);
    void setXUnits(const QString &newUnits);
    void setTimeDisplayed(int seconds);
    void setSeriesColor(const QColor &color);
    void setUpdateGranularity(int newVal);
    void setMetaDataHeaders();
    void saveCheck();
    void dataVectorSizeCheck();

protected slots:
    /// When the pv indicates there's been a value update, this function handles recording the new value as well as updating the displayed values.
    void onPVValueChanged(double newValue);
//    void toForceUpdateValue(const QString &toIgnore);
    void toUpdateTime(int newTime);
    void toUpdateTimeUnits(const QString &newUnits);

    void toRestartUpdateIntervalTimer();
    void toManuallyUpdatePV();

};

#endif // STRIPTOOLPV_H
