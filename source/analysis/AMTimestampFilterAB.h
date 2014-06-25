//#ifndef AMTIMESTAMPFILTERAB_H
//#define AMTIMESTAMPFILTERAB_H

//#include <QObject>
//#include <QDebug>

//#include "analysis/AMStandardAnalysisBlock.h"
//#include "analysis/AM0DTimestampAB.h"

//class AMTimestampFilterAB : public AMStandardAnalysisBlock
//{
//    Q_OBJECT

//public:
//    /// Constructor.
//    Q_INVOKABLE AMTimestampFilterAB(const QString &outputName, QObject *parent = 0);
//    /// Destructor.
//    virtual ~AMTimestampFilterAB();

//    /// Returns the current time value.
//    int timeValue() const;

//    /// Check if a set of inputs is valid. The empty list (no inputs) must always be valid. For non-empty lists, our specific requirements are...
//    /*! - there must be one input source.
//    - the rank() of the input source must be 1.
//    */
//    virtual bool areInputDataSourcesAcceptable(const QList<AMDataSource*>& dataSources) const;

//    /// Set the data source inputs.
//    virtual void setInputDataSourcesImplementation(const QList<AMDataSource*>& dataSources);

//    /// Returns the dependent value at a (complete) set of axis indexes. Returns an invalid AMNumber if the indexes are insuffient or any are out of range, or if the data is not ready.
//    virtual AMNumber value(const AMnDIndex &indexes) const;

//    /// Performance optimization of value(): instead of a single value, copies a block of values from \c indexStart to \c indexEnd (inclusive), into \c outputValues.  The values are returned in row-major order (ie: with the first index varying the slowest). Returns false if the indexes have the wrong dimension, or (if AM_ENABLE_BOUNDS_CHECKING is defined, the indexes are out-of-range).
//    /*! 	It is the caller's responsibility to make sure that \c outputValues has sufficient size.  You can calculate this conviniently using:

//    \code
//    int outputSize = indexStart.totalPointsTo(indexEnd);
//    \endcode
//    */
//    virtual bool values(const AMnDIndex& indexStart, const AMnDIndex& indexEnd, double* outputValues) const;

//    /// When the independent values along an axis is not simply the axis index, this returns the independent value along an axis (specified by axis number and index)
//    virtual AMNumber axisValue(int axisNumber, int index) const;

//    /// Re-implemented from AMDbObject to set the AMDataSource name once we have an AMDbObject::name()
//    bool loadFromDb(AMDatabase *db, int id);

//signals:
//    void timeValueChanged(int newValue);

//public slots:
//    /// Sets the time value.
//    void setTimeValue(int newValue);

//protected slots:
//    /// Connected to be called when the values of the input data source change
//    void onInputSourceValuesChanged(const AMnDIndex& start, const AMnDIndex& end);
//    /// Connected to be called when the state() flags of any input source change
//    void onInputSourceStateChanged();

//protected:
//    /// Helper function to look at our overall situation and determine what the output state should be.
//    void reviewState();

//protected:
//    int timeValue_;
//    QVector<double> values_;
//    QVector<double> times_;

//};

//#endif // AMTIMESTAMPFILTERAB_H
