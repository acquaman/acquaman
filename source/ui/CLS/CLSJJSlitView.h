#ifndef CLSJJSLITVIEW_H
#define CLSJJSLITVIEW_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QLabel>

class CLSJJSlit;
class AMExtendedControlEditor;

class CLSJJSlitView : public QWidget
{
    Q_OBJECT
public:
	/// Constructor
	explicit CLSJJSlitView(CLSJJSlit *clsJJSlit, QWidget *parent = 0);
	/// Destructor.
	virtual ~CLSJJSlitView();

protected slots:
	/// handle the signal of CLSJJSlit connected, prefill the values to the spinbox
	void onJJSlitConnected();

protected:
	/// the instance of CLSJJSlit
	CLSJJSlit * clsJJSlit_;

	AMExtendedControlEditor *verticalGapEditor_;
	AMExtendedControlEditor *verticalCenterEditor_;
	AMExtendedControlEditor *horizontalGapEditor_;
	AMExtendedControlEditor *horizontalCenterEditor_;

//	/// the spinbox for vertical Gap
//	QDoubleSpinBox * verticalGapSpinBox_;
//	/// the spinbox for vertical Center
//	QDoubleSpinBox * verticalCenterSpinBox_;
//	/// the spinbox for vertical Status
//	QLabel * verticalStatusLabel_;

//	/// the spinbox for horizontal Gap
//	QDoubleSpinBox * horizontalGapSpinBox_;
//	/// the spinbox for horizontal Center
//	QDoubleSpinBox * horizontalCenterSpinBox_;
	/// the spinbox for horizontal Status
//	QLabel * horizontalStatusLabel_;
};

#endif // CLSJJSLITVIEW_H
