#ifndef AMSHAPEDATA_H
#define AMSHAPEDATA_H

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
    /// constructors
    AMShapeData();
    AMShapeData(QPolygonF shape, QString name = "Name", QString otherData = "",  double idNumber = 0);

    /// accessors
    QPolygonF* shape();
    QString name();
    QString otherData();
    double idNumber();
    QVector3D coordinate(int index);
    double height();
    double width();
    double rotation();
    double tilt();
    double yAxisRotation();

    /// mutators
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
    void setYAxisRotation(double yAxisRotation);

    /// returns the center
    QVector3D centerCoordinate();

    /// shifts the shape by shift
    void shift(QVector3D shift);
    /// shifts the shape to shift
    void shiftTo(QVector3D shift);

    /// number of coordinates
    int count();

    /// checks the direction
    bool backwards();

protected:
    /// the 2D shape to store
    QPolygonF* shape_;
    /// the shapes name
    QString name_;
    /// other data
    QString otherData_;
    /// an id number
    double idNumber_;
    /// the 3D shape to store
    QVector<QVector3D> coordinate_;
    /// height of the shape
    double height_;
    /// width of the shape
    double width_;
    /// z-axis rotation
    double rotation_;
    /// x-axis rotation
    double tilt_;
    /// y-axis rotation
    double yAxisRotation_;
    /// the number of coordinates
    int coordinateCount_;

    bool validIndex(int index);

};

#endif // AMSHAPEDATA_H
