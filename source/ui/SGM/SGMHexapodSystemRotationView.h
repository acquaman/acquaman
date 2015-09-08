#ifndef SGMHEXAPODSYSTEMROTATIONVIEW_H
#define SGMHEXAPODSYSTEMROTATIONVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
class SGMHexapod;

/*!
  * A ui class which allows users to tweak the rotation of the coordinate system
  * of the hexapod.
  */
class SGMHexapodSystemRotationView : public QWidget
{
    Q_OBJECT
public:
	/*!
	  * Creates an instance of a hexapod system rotation view which will allow
	  * for altering the rotation of the provided hexapod's coordinate system
	  */
	explicit SGMHexapodSystemRotationView(SGMHexapod* hexapod, QWidget *parent = 0);

signals:

public slots:
protected slots:
	/*!
	  * Handles signals indicating that the apply rotation button has been clicked.
	  * Applies the rotation within the tranform spinboxes, then resets their values
	  * to zero.
	  */
	void onApplyButtonClicked();

	/*!
	  * Handles signals indicating that the reset rotation button has been clicked.
	  * Resets the rotation of the hexapod system back to its default value of
	  * in the plane of the sample plate.
	  */
	void onResetButtonClicked();
protected:
	/*!
	  * Initializes the ui components, and connects the relevant signals.
	  */
	void setupUI();

	SGMHexapod* hexapod_;

	QPushButton* applyRotationButton_;
	QPushButton* resetRotationButton_;

	QLabel* xAngleTransformLabel_;
	QLabel* yAngleTransformLabel_;
	QLabel* zAngleTransformLabel_;

	QDoubleSpinBox* xAngleTransformSpinbox_;
	QDoubleSpinBox* yAngleTransformSpinbox_;
	QDoubleSpinBox* zAngleTransformSpinbox_;
};

#endif // SGMHEXAPODSYSTEMROTATIONVIEW_H
