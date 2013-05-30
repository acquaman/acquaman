#ifndef AMSHAPEDATAVIEW_H
#define AMSHAPEDATAVIEW_H

#include <QWidget>

class AMShapeData;
class QLineEdit;
class QPushButton;

class AMShapeDataView : public QWidget
{
    Q_OBJECT
public:
    explicit AMShapeDataView(AMShapeData *shapeModel = 0, QWidget *parent = 0);

public slots:
    void setName(QString);
    void setTilt(QString);
    void setX(QString);
    void setY(QString);
    void setZ(QString);
    void setRotation(QString);
    void setShapeData(AMShapeData*);



    void nameChanged(QString);
    void tiltChanged(QString);
    void xChanged(QString);
    void yChanged(QString);
    void zChanged(QString);

    void rotationChanged(QString);


signals:
    void updateShapes();
    void setCoordinate();
    void applyDistortion();
protected:
    void update();
    bool isValid();
protected:
    AMShapeData *shapeModel_;
    QLineEdit *nameEdit_;
    QLineEdit *tiltEdit_;
    QLineEdit *xEdit_;
    QLineEdit *yEdit_;
    QLineEdit *zEdit_;
    QLineEdit *rotationEdit_;
    QPushButton *setCoordinate_;
    QPushButton *distortionButton_;
};

#endif // AMSHAPEDATAVIEW_H
