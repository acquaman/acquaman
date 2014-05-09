//#ifndef STVECTORDATACOLLECTION_H
//#define STVECTORDATACOLLECTION_H

//#include "StripTool2/STDataCollection.h"
//#include "MPlot/MPlotSeriesData.h"

//class STVectorDataCollection : public STDataCollection
//{
//    Q_OBJECT

//public:
//    explicit STVectorDataCollection(const QString &name, QObject *parent = 0);
//    virtual ~STVectorDataCollection();

//    int dataSize() const;
//    int storeSize() const;
//    int displaySize() const;

//    QVector<QTime &> timeModel() const;
//    QVector<double> valueModel() const;
//    MPlotVectorSeriesData *displayModel() const;

//signals:
//    void storeSizeChanged(int newStoreSize);
//    void displaySizeChanged(int newDisplaySize);

//    void dataAdded();

//public slots:
//    void setStoreSize(int newStoreSize);
//    void setDisplaySize(int newDisplaySize);

//    virtual void addData(QTime &time, double value);
//    void onDataAdded();

//protected:
//    void modelsRemoveFirst();
//    void modelsShuffleDown();
//    void modelsAddLast(QTime &time, double value);
//    void modelsReplaceLast(QTime &time, double value);

//    void updateDisplayModel();

//protected:
//    /// the number of valid data points collected in the time model and value model.
//    int dataSize_;
//    /// the size of the time model and the value model.
//    int storeSize_;
//    /// the number of valid points in the time/value models to display in the display model.
//    int displaySize_;

//    QVector<QTime&> timeModel_;
//    QVector<double> valueModel_;
//    MPlotVectorSeriesData *displayModel_;

//};

//#endif // STVECTORDATACOLLECTION_H
