#ifndef AMSHAPEDATA_H
#define AMSHAPEDATA_H

#include <QObject>
#include <QRectF>
#include <QVector3D>
#include <QVector>
#include <QPolygonF>


class QString;
class QVector3D;


/// This class holds a 3D shape and a 2D shape.
/// The 2D shape is meant to correspond to a visualization
/// of the 3D shape.
class AMShapeData : public QObject
{
    Q_OBJECT
public:
    /// constructors
    AMShapeData(QObject* parent = 0);
    AMShapeData(QPolygonF shape, QString name = "Name", QString otherDataFieldOne = "", QString otherDataFieldTwo = "",  double idNumber = 0, QObject* parent = 0);

    ~AMShapeData();

    /// accessors
    QPolygonF* shape() const;
    QString name() const;
    QString otherDataFieldOne() const;
    QString otherDataFieldTwo() const;
    double idNumber() const;
    QVector3D coordinate(int index) const;
    QVector<QVector3D> coordinates() const;
    double rotation() const;
    double tilt() const;
    double yAxisRotation() const;
    bool visible() const;


    void copy(const AMShapeData* other);

    /// returns the center
    QVector3D centerCoordinate() const;

    /// shifts the shape by shift
    void shift(QVector3D shift);
    /// shifts the shape to shift
    void shiftTo(QVector3D shift);

    /// number of coordinates
    int count() const;

    /// checks the direction
    bool backwards();

    bool isEqual(const AMShapeData& other) const;

public slots:

    /// mutators
    void setShape(const QPolygonF shape);
    void setName(QString name);
    void setOtherDataFieldOne(QString otherDataFieldOne);
    void setOtherDataFieldTwo(QString otherDataFieldTwo);
    void setIdNumber(double idNumber);
    void setCoordinate(QVector3D coordinate, int index);
    void setCoordinateShape(QVector<QVector3D> coordinates, int count);
    void setRotation(double zAxisRotation);
    void setTilt(double xAxisRotation);
    void setYAxisRotation(double yAxisRotation);
    void setVisible(bool visible);

signals:
    void nameChanged(QString name);
    void otherDataFieldOneChanged(QString data);
    void otherDataFieldTwoChanged(QString data);
    void idNumberChanged(double idNumber);

protected:
    /// the 2D shape to store
    QPolygonF* shape_;
    /// the shapes name
    QString name_;
    /// other data
    QString otherDataFieldOne_;
    QString otherDataFieldTwo_;
    /// an id number
    double idNumber_;
    /// the 3D shape to store
    QVector<QVector3D> coordinate_;
    /// z-axis rotation
    double zAxisRotation_;
    /// x-axis rotation
    double xAxisRotation_;
    /// y-axis rotation
    double yAxisRotation_;
    /// the number of coordinates
    int coordinateCount_;

    bool visible_;

    bool validIndex(int index) const;

};

#endif // AMSHAPEDATA_H
