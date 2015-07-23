#ifndef CLSSTANDARDSWHEELCONFIGURATIONVIEW_H
#define CLSSTANDARDSWHEELCONFIGURATIONVIEW_H

#include <QWidget>

#include "beamline/CLS/CLSStandardsWheel.h"

#include <QLineEdit>
#include <QComboBox>

/// This class encapsulates a single standards wheel piece.
class CLSStandardsWheelConfigurationViewElement : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes a standards wheel element.
	explicit CLSStandardsWheelConfigurationViewElement(CLSStandardsWheelElement *element, QWidget *parent = 0);
	/// Destructor.
	~CLSStandardsWheelConfigurationViewElement();

	/// Returns the element.
	CLSStandardsWheelElement *wheelElement() const { return wheelElement_; }

protected slots:
	/// Handles setting the name after editing.
	void onNameChanged();

protected:
	/// The element being visualized.
	CLSStandardsWheelElement *wheelElement_;
	/// The line edit where you can modify the name.
	QLineEdit *nameEdit_;
};

/// This class handles configuring the wheel it represents.  At the moment, this is only the name.
class CLSStandardsWheelConfigurationView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit CLSStandardsWheelConfigurationView(CLSStandardsWheel *standardsWheel, QWidget *parent = 0);
	/// Destructor.
	~CLSStandardsWheelConfigurationView();

	/// Returns the CLS standards wheel model this view represents.
	CLSStandardsWheel *standardsWheel() const { return standardsWheel_; }

signals:

public slots:

protected slots:
	/// Test slot for the standards wheel.
	void onStandardsWheelIndexChanged(int index);
	/// Handles updating the standards wheel names.
	void onStandardsWheelNameChanged(int index, const QString &newName);
	/// Handles updating the combo box with the new position of the standards wheel.
	void onStandardsWheelPositionChanged(int index);

protected:
	/// The wheel to be visualized.
	CLSStandardsWheel *standardsWheel_;

	/// Combo box for the standards wheel.
	QComboBox *standardsComboBox_;
};

#endif // CLSSTANDARDSWHEELCONFIGURATIONVIEW_H
