#ifndef AM1DDARKCURRENTCORRECTIONAB_H
#define AM1DDARKCURRENTCORRECTIONAB_H


#include "analysis/AMStandardAnalysisBlock.h"

/*!
  This analysis block accepts two or more 1D data sources and input dark current correction value.
  The output is the measurement value scaled in time by the time value, corrected by the dark current value.
  The output data is the same size as the input sources.  There is an assumed order for the
  data sources and that is:

        1) Data
        2) Dwell time per point
        3) (not actually a data source) dark current correction value in counts/sec.

The actual data source used by ??? is determined by the analyzed name provided.
  */
class AM1DDarkCurrentCorrectionAB : public AMStandardAnalysisBlock
{
    Q_OBJECT

    Q_PROPERTY(QString dataName READ dataName WRITE setDataName)
    Q_PROPERTY(QString dwellTimeName READ dwellTimeName WRITE setDwellTimeName)
    Q_PROPERTY(double darkCurrent READ darkCurrent WRITE setDarkCurrent)
    Q_PROPERTY(double timeUnitMultiplier READ timeUnitMultiplier WRITE setTimeUnitMultiplier)

    Q_CLASSINFO("AMDbObject_Attributes", "description=1D Normalization Block")

public:
    /// Constructor.
    Q_INVOKABLE AM1DDarkCurrentCorrectionAB(const QString &outputName = "InvalidInput", QObject *parent = 0);
    virtual ~AM1DDarkCurrentCorrectionAB();

    QString infoDescription() const { return QString(); }

    /// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
    /*! - two 1D data sources
        - For proper results, order must be the data source with real data and the second must be the dwell time.
        */
    virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

    /// Set the data source inputs.
    virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

    /// Set the analyzed data source name.
    void setDataName(const QString &name);
    /// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
    QString dataName() const { return dataName_; }
    /// Set the analyzed data source name.
    void setDwellTimeName(const QString &name);
    /// Returns the current analyzed data source name.  If none have been set then this returns an empty string.
    QString dwellTimeName() const { return dwellTimeName_; }
    /// Returns whether the data source can be evaluated.  Checks against the current analyzed name.
    bool canAnalyze() const { return canAnalyze_; }
    /// Returns whether the data source can be evaluated by passing in both names, \param dataName and \param dwellTimeName.  Even though, the analysis block can be evaluated regardless of the name if there is only one data source, this will return true even if the name doesn't match.
    bool canAnalyze(const QString &dataName, const QString &dwellTimeName) const;

    /// Returns the current dark current value in units/time (you need to pick the appropriate units of time).
    double darkCurrent() const {return darkCurrent_;}
    /// Set the time unit multiplier, for the case where the time units for measurement and for dark current value are not the same.
    void setTimeUnitMultiplier(double newMultiplier);
    double timeUnitMultiplier() const {return timeUnitMultiplier_;}

    // Data Retrieval

    /// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
    virtual AMNumber value(const AMnDIndex& indexes) const;
    /// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
    /*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

    \code
    int outputSize = indexStart.totalPointsTo(indexEnd);
    \endcode
    */
    virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;
    /// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
    virtual AMNumber axisValue(int axisNumber, int index) const;


    //////////////////////////////////////////////

    /// Reimplement the create widget method.
    QWidget *createEditorWidget();

    /// Re-implemented from AMDbObject to set the AMDataSource name once we have an AMDbObject::name()
    bool loadFromDb(AMDatabase *db, int id);

public slots:
    /// Set the dark current correction value.
    void setDarkCurrent(double newValue);

protected slots:
    /// Connected to be called when the values of the input data source change
    void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
    /// Connected to be called when the size of the input source changes
    void onInputSourceSizeChanged();
    /// Connected to be called when the state() flags of any input source change
    void onInputSourceStateChanged();

protected:
    /// Helper function to look at our overall situation and determine what the output state should be.
    void reviewState();
    /// Helper method that sets the data_ and normalizer_ pointer to the correct data source based on the current state of analyzedName_.
    void setInputSources();

    /// Pointer to the data source that will be analyzed.
    AMDataSource *data_;
    /// Pointer to the data source that is the dwell time.
    AMDataSource *dwellTime_;
    /// Pointer to the dark current correction value.
    double darkCurrent_;

    /// The name of the data source that should be analyzed.
    QString dataName_;
    /// The name of the data source that be used for dwell time.
    QString dwellTimeName_;
    /// Flag holding whether or not the data source can be analyzed.
    bool canAnalyze_;

    double timeUnitMultiplier_;
};


#endif // AM1DDARKCURRENTCORRECTIONAB_H