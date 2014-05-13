#ifndef AMSHAPEDATAVIEW_H
#define AMSHAPEDATAVIEW_H

#include <QWidget>

class AMShapeData;
class QLineEdit;
class QPushButton;
class QSlider;
class AMSampleAdvancedView;
class QFrame;
class QScrollArea;

#define AMSHAPEDATAVIEW_UPDATEALL_FAILED_NOT_VALID 603001

/// This class is a view for the AMShapeData class.
class AMShapeDataView : public QWidget
{
Q_OBJECT
public:
	explicit AMShapeDataView(AMShapeData *shapeModel = 0, QWidget *parent = 0);

public slots:
	void setTilt(QString);
	void setX(QString);
	void setY(QString);
	void setZ(QString);
	void setRotation(QString);
	void setShapeData(AMShapeData*);
	void setYAxisRotation(QString);

	void nameChanged(QString);
	void tiltChanged(QString);
	void xChanged(QString);
	void yChanged(QString);
	void zChanged(QString);

	void rotationChanged(QString);
	void yAxisRotationChanged(QString);

	void toggleShapeVisible();
	void setShapeVisible(bool visible);
	void showSampleView();
	void setCoordinate();

signals:
	void updateShapes();
	void applyDistortion();
	void newName();
	void shapeVisible(bool);

protected slots:
	void xAxisRotation(int);
	void yAxisRotation(int);
	void zAxisRotation(int);
	void updateAll();

protected:
	bool isValid();
	void updateCoordinateLabels();
	int count();

protected:
	AMShapeData *shapeModel_;
	QLineEdit *tiltEdit_;
	QLineEdit *xEdit_;
	QLineEdit *yEdit_;
	QLineEdit *zEdit_;
	QLineEdit *rotationEdit_;
	QLineEdit * yRotationEdit_;
	QSlider* xAxisSlider_;
	QSlider* yAxisSlider_;
	QSlider* zAxisSlider_;
	QPushButton* showHideButton_;

	QLineEdit** coordinateEdit_;
	QFrame* coordinateFrame_;
	QScrollArea* scrollArea_;

	int oldCount_;
};

#endif // AMSHAPEDATAVIEW_H
