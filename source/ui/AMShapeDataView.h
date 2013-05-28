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
    explicit AMShapeDataView(QWidget *parent = 0);

public slots:
    void setName(QString);
    void setTilt(QString);
    void setX(QString);
    void setY(QString);
    void setZ(QString);
    void setRotation(QString);

signals:
    void nameChanged(QString);
    void tiltChanged(QString);
    void xChanged(QString);
    void yChanged(QString);
    void zChanged(QString);
    void rotationChanged(QString);
    void setCoordinate();
    void applyDistortion();
    //protected slots
    //protected (fns)
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
