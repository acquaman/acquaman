#ifndef AMSHAPEDATA2_H
#define AMSHAPEDATA2_H

#include <QObject>
#include <QRectF>


class QString;

class AMShapeData2 //: public QObject
{
   // Q_OBJECT
public:
    AMShapeData2();
    AMShapeData2(QRectF rectangle, QString name = "Name", QString otherData = "",  double idNumber = 0);
    QRectF* rectangle();
    QString name();
    QString otherData();
    double idNumber();
    void setRectangle(QRectF rectangle);
    void setName(QString name);
    void setOtherData(QString otherData);
    void setIdNumber(double idNumber);

private:
    QRectF* rectangle_;
    QString name_;
    QString otherData_;
    double idNumber_;
};

#endif // AMSHAPEDATA2_H
