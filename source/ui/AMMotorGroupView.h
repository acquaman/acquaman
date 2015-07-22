/*
Copyright 2010-2012 Mark Boots, David Chevrier, and Darren Hunter.
Copyright 2013-2014 David Chevrier and Darren Hunter.

This file is part of the Acquaman Data Acquisition and Management framework ("Acquaman").

Acquaman is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Acquaman is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Acquaman.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef AMMOTORGROUPVIEW_H
#define AMMOTORGROUPVIEW_H

#include <QWidget>
#include <QLabel>
#include <QToolButton>
#include <QTabWidget>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QSignalMapper>

#include "beamline/AMMotorGroup.h"

class AMMotorGroupObjectView : public QWidget
{
	Q_OBJECT

public:
	explicit AMMotorGroupObjectView(
			AMMotorGroupObject* motorGroupObject,
			QWidget* parent = 0);

	virtual ~AMMotorGroupObjectView() {}

protected slots:
	void onConnectionStateChanged(AMMotorGroupObject::MotionDirection direction,
								  AMMotorGroupAxis::MotionType motionType,
								  bool isConnected);

	void onMotorError(AMMotorGroupObject::MotionDirection direction,
					  AMMotorGroupAxis::MotionType motionType,
					  int errorCode);

	void onMovingStateChanged(AMMotorGroupObject::MotionDirection direction,
							  AMMotorGroupAxis::MotionType motionType,
							  bool isMoving);

	void onPositionUnitsChanged(AMMotorGroupObject::MotionDirection direction,
								AMMotorGroupAxis::MotionType motionType,
								const QString& positionUnits);

	void onPositionValueChanged(AMMotorGroupObject::MotionDirection direction,
								AMMotorGroupAxis::MotionType motionType,
								double positionValue);

	void onShowLogButtonClicked();
protected:

	void setupUi();
	void setupData();
	void setupConnections();

	virtual void errorHandleImplementation(AMMotorGroupObject::MotionDirection direction,
										   AMMotorGroupAxis::MotionType motionType,
										   int errorCode);

	// Model Reference
	AMMotorGroupObject* motorGroupObject_;


	// Widgets
	QLabel* titleLabel_;

	QTextEdit* logConsole_;
	QToolButton* showLogButton_;

	QToolButton* horizontalTranslationIncrement_;
	QToolButton* horizontalTranslationDecrement_;
	QDoubleSpinBox* horizontalTranslationValue_;
	QLabel* horizontalTranslationLabel_;
	QToolButton* horizontalRotationIncrement_;
	QToolButton* horizontalRotationDecrement_;
	QDoubleSpinBox* horizontalRotationValue_;
	QLabel* horizontalRotationLabel_;

	QToolButton* verticalTranslationIncrement_;
	QToolButton* verticalTranslationDecrement_;
	QDoubleSpinBox* verticalTranslationValue_;
	QLabel* verticalTranslationLabel_;
	QToolButton* verticalRotationIncrement_;
	QToolButton* verticalRotationDecrement_;
	QDoubleSpinBox* verticalRotationValue_;
	QLabel* verticalRotationLabel_;

	QToolButton* normalTranslationIncrement_;
	QToolButton* normalTranslationDecrement_;
	QDoubleSpinBox* normalTranslationValue_;
	QLabel* normalTranslationLabel_;
	QToolButton* normalRotationIncrement_;
	QToolButton* normalRotationDecrement_;
	QDoubleSpinBox* normalRotationValue_;
	QLabel* normalRotationLabel_;

	QToolButton* stopAllButton_;
};



class AMMotorGroupView : public QWidget
{
	Q_OBJECT
public:
	explicit AMMotorGroupView(AMMotorGroup* motorGroup_, QWidget* parent = 0);

	virtual ~AMMotorGroupView() {}

protected slots:
	void onGroupObjectMotionStatusAltered(const QString& groupObjectName);
protected:

	AMMotorGroup* motorGroup_;
	QTabWidget* groupObjectTabs_;
	QHash<QString, int>	 motorGroupTabMap_;
	QSignalMapper* groupMotionStatusMapper_;
};
#endif // AMMOTORGROUPVIEW_H
