#ifndef AMSHAPEDATA2_H
#define AMSHAPEDATA2_H

#include <QObject>
#include <QRectF>
#include <QVector3D>


class QString;
class QVector3D;

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
    QVector3D coordinate();
    double height();
    double width();
    double rotation();
    void setRectangle(QRectF rectangle);
    void setName(QString name);
    void setOtherData(QString otherData);
    void setIdNumber(double idNumber);
    void setCoordinate(QVector3D coordinate);
    void setHeight(double height);
    void setWidth(double width);
    void setRotation(double rotation);

private:
    QRectF* rectangle_;
    QString name_;
    QString otherData_;
    double idNumber_;
    QVector3D coordinate_;
    double height_;
    double width_;
    double rotation_;


};

#endif // AMSHAPEDATA2_H
