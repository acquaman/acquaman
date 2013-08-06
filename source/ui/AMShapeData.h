#ifndef AMSHAPEDATA_H
#define AMSHAPEDATA_H

#include <QObject>
#include <QRectF>
#include <QVector3D>
#include <QVector>
#include "AMSampleEthan.h"


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

    ~AMShapeData();

    /// accessors
    QPolygonF* shape() const;
    QString name() const;
    QString otherData() const;
    double idNumber() const;
    QVector3D coordinate(int index) const;
    double height() const;
    double width() const;
    double rotation() const;
    double tilt() const;
    double yAxisRotation() const;
    bool visible() const;

    AMSampleEthan* sample() const;

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
    void setVisible(bool visible);

    void setSample(AMSampleEthan* sample);

    /// returns the center
    QVector3D centerCoordinate() const;

    /// shifts the shape by shift
    void shift(QVector3D shift);
    /// shifts the shape to shift
    void shiftTo(QVector3D shift);

    /// number of coordinates
    int count();

    /// checks the direction
    bool backwards();

    bool operator==(const AMShapeData& other) const;

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

    bool visible_;

    AMSampleEthan* sample_;

    bool validIndex(int index) const;

};

#endif // AMSHAPEDATA_H
