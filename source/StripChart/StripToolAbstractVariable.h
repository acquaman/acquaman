//#ifndef STRIPTOOLABSTRACTVARIABLE_H
//#define STRIPTOOLABSTRACTVARIABLE_H

//#include <QObject>

//#include "dataman/datasource/AMDataSource.h"
//#include "StripChart/StripToolAbstractVariableInfo.h"

//class AMnDIndex;
//class MPlotItem;

//class StripToolAbstractVariable : public QObject
//{
//    Q_OBJECT

//public:
//    explicit StripToolAbstractVariable(StripToolAbstractVariableInfo *info, AMDataSource *dataSource, QObject *parent = 0);

//signals:
//    void validStateChanged(bool isValid);

//public:
//    bool operator== (const StripToolAbstractVariable* anotherVariable) const;
//    bool isValid() const;
//    QString name() const;
//    virtual MPlotItem* plotItem() const = 0;

//protected:
//    void setInfo(StripToolAbstractVariableInfo *newInfo);
//    virtual StripToolAbstractVariableInfo* info() const;

//    void setDataSource(AMDataSource *newSource);
//    virtual AMDataSource* dataSource() const;

//protected slots:
//    void onValidStateChanged(bool isValid);

//private slots:
//    virtual void onInfoChanged() = 0;
//    virtual void onDataSourceValuesChanged(const AMnDIndex &start, const AMnDIndex &end) = 0;

//private:
//    void setName(const QString &sourceName);
//    void setDescription(const QString &newDescription);

//private:
//    StripToolAbstractVariableInfo *info_;
//    AMDataSource *dataSource_;

//    bool infoIsValid_;
//    bool dataSourceIsValid_;

//};

//#endif // STRIPTOOLABSTRACTVARIABLE_H
