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
    /// Pv signals to the model when it is time to save pv data to file.
    void savePVData();
    void savePVMetaData();
    void forceUpdate(double newValue);
    void pvValueUpdated();

protected:
    int updateIndex_;

    int defaultTimeDisplayed_;
    int timeDisplayed_;

    int dataVectorSize_;
    int savePoint_;

    QList<QString> headers_;

    QString pvName_;
    QString pvDescription_;
    QString xUnits_;
    QString yUnits_;
    bool isUpdating_;
    Qt::CheckState checkState_;
    QColor pvColor_;

    double timeFactor_;

    QVector<QTime> masterUpdateTimes_;
    QVector<double> masterUpdateValues_;

    QVector<double> displayedTimes_;
    QVector<double> displayedValues_;

    MPlotVectorSeriesData *pvData_;
    MPlotSeriesBasic *pvSeries_;
    AMControl *pvControl_;

    bool forceUpdate_;

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
    /// Returns a pointer to the pv's instance of MPlotVectorSeriesData. This was useful earlier, when a pv's series was actually a property of the plot, but I don't think it's used now.
    MPlotVectorSeriesData* data();
    /// Returns a pointer to the series that should be plotted for this pv.
    MPlotSeriesBasic* series();
    int timeDisplayed();
    /// This is here because I thought it would be neat to have the pv data saved periodically, which would be handled by the model. This could cut down on memory needed, if this application runs for a long time but isn't fully implemented.
    QVector<QString> saveMasterTimes();
    QVector<double> saveMasterValues();
    QList<QString> metaDataHeaders();
    QList<QString> metaData();
    bool setMetaData(QList<QString> metaData);

    /// Sets the PV control and reparents for this pv.
    void setControl(AMControl *newControl);

    /// The user can pause and restart the value updates displayed on the plot (this class will still continue to record updates).
    void setPVUpdating(bool isUpdating);
    /// The list view will allow the user to un/check a pv, and the most recent update for a pv's check state is set here.
    void setCheckState(Qt::CheckState isChecked);

    bool operator== (const StripToolPV &anotherPV);

private:
    void setDescription(const QString &newDescription);
    void setYUnits(const QString &newUnits);
    void setXUnits(const QString &newUnits);
    void setTimeDisplayed(int seconds);
    void setSeriesColor(const QColor &color);
    void setMetaDataHeaders();
    void saveCheck();
    void dataVectorSizeCheck();

protected slots:
    /// When the pv indicates there's been a value update, this function handles recording the new value as well as updating the displayed values.
    void onPVValueChanged(double newValue);
    void toForceUpdateValue(const QString &toIgnore);
    void toUpdateTime(int newTime);
    void toUpdateTimeUnits(const QString &newUnits);

};

#endif // STRIPTOOLPV_H
