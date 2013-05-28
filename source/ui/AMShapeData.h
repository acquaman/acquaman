#ifndef AMSHAPEDATA2_H
#define AMSHAPEDATA2_H

#include <QObject>
#include <QRectF>
#include <QVector3D>
#include <QVector>


class QString;
class QVector3D;
class QPolygonF;

class AMShapeData //: public QObject
{
   // Q_OBJECT
public:
    AMShapeData();
    AMShapeData(QPolygonF shape, QString name = "Name", QString otherData = "",  double idNumber = 0);
    QPolygonF* shape();
    QString name();
    QString otherData();
    double idNumber();
    QVector3D coordinate(int index);
    double height();
    double width();
    double rotation();
    double tilt();
    void setShape(QPolygonF shape);
    void setName(QString name);
    void setOtherData(QString otherData);
    void setIdNumber(double idNumber);
    void setCoordinate(QVector3D coordinate, int index);
    void setCoordinateShape(QVector<QVector3D> coordinates, int count);
    void setHeight(double height);
    void setWidth(double width);
    void setRotation(double rotation);
    void setTilt(double tilt);

protected:
    QPolygonF* shape_;
    QString name_;
    QString otherData_;
    double idNumber_;
    QVector<QVector3D> coordinate_;
    double height_;
    double width_;
    double rotation_;
    double tilt_;
    int coordinateIndex_;

    bool validIndex(int index);

};

#endif // AMSHAPEDATA2_H
