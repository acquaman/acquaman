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
    StripToolPV(const QString &pvName, const QString &pvDescription = "", const QString &pvUnits = "", QObject *parent = 0);
    ~StripToolPV();
    friend class StripToolModel;

signals:
    /// Not presently used, but if saving pv data to a file was handled automatically, then the pv could signal to the model when it was time for the write to happen.
    void savePV();

protected:
    int updateIndex_;
    int defaultValuesDisplayed_;
    int valuesDisplayed_;
    int dataVectorSize_;
    QString pvName_;
    QString pvDescription_;
    QString xUnits_;
    QString yUnits_;
    bool isUpdating_;
    Qt::CheckState checkState_;
    QColor pvColor_;
    QVector<double> pvUpdateIndex_;
    QVector<double> pvDataTotal_;
    QVector<double> xValuesDisplayed_;
    QVector<double> yValuesDisplayed_;
    MPlotVectorSeriesData *pvData_;
    MPlotSeriesBasic *pvSeries_;
    AMControl *pvControl_;

protected:
    /// Returns the epics pv name for this pv.
    QString pvName() const;
    /// Returns the description for this pv, if the user has provided one.
    QString pvDescription();
    /// The x units are the same for all pvs right now--"Update number"--but if they were different, they could be found here.
    QString xUnits();
    /// Returns the pv's units, if the user has provided them.
    QString yUnits();
    /// Returns the checkstate for this pv : checked indicates it should appear on the plot, unchecked that it should be hidden.
    Qt::CheckState checkState();
    /// Returns the color set for this pv's series.
    QColor color();
    /// Returns a pointer to the pv's instance of MPlotVectorSeriesData. This was useful earlier, when a pv's series was actually a property of the plot, but I don't think it's used now.
    MPlotVectorSeriesData* data();
    /// Returns a pointer to the series that should be plotted for this pv.
    MPlotSeriesBasic* series();
    /// Returns an integer representing the number of pv value updates that are displayed on the plot. By default, it displays the last ten updates.
    int valuesDisplayed();
    /// This is here because I thought it would be neat to have the pv data saved periodically, which would be handled by the model. This could cut down on memory needed, if this application runs for a long time but isn't fully implemented.
    QVector<double> saveData();
    /// Returns the bottom axis range for the values displayed for this pv.
    MPlotAxisRange axisBottomRange() const;

    /// Sets the PV control and reparents for this pv.
    void setControl(AMControl *newPV);
    /// If the user wants to set a new description for the pv, this is where it happens.
    void setDescription(const QString &newDescription);
    /// If the user provides units for a pv, this is where that entry is stored.
    void setUnits(const QString &newUnits);
    /// The user can pause and restart the value updates displayed on the plot (this class will still continue to record updates).
    void setPVUpdating(bool isUpdating);
    /// The user can change the number of value updates displayed on the plot at a time.
    void setValuesDisplayed(int points);
    /// There used to be a tool with "show more" and "show less" type capabilities--the user could increment the number of updates shown. I'm not sure this is being used anymore.
    void incrementValuesDisplayed(int diff);
    /// The list view will allow the user to un/check a pv, and the most recent update for a pv's check state is set here.
    void setCheckState(Qt::CheckState isChecked);
    /// The user can change the color that the pv appears in on the plot.
    void setSeriesColor(const QColor &color);

    bool operator== (const StripToolPV &anotherPV);

protected slots:
    /// When the pv indicates there's been a value update, this function handles recording the new value as well as updating the displayed values.
    void onPVValueChanged(double newValue);

};

#endif // STRIPTOOLPV_H
