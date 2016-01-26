#ifndef BIOXASZEBRAVIEW_H
#define BIOXASZEBRAVIEW_H

#include <QTabWidget>

#include "beamline/BioXAS/BioXASZebra.h"

#include <QPushButton>

/// This is the main widget view for looking at the Zebra in Acquaman.
class BioXASZebraView : public QTabWidget
{
	Q_OBJECT

public:
	/// Constructor.  Takes a zebra widget.
	explicit BioXASZebraView(BioXASZebra *zebra, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASZebraView();

signals:

public slots:

protected slots:
	/// Slot that handles setting the soft input 1 control.
	void onSoftInput1Toggled(bool state);
	/// Slot that handles updating the checked state of the soft input 1 button.
	void updateSoftInput1();
	/// Slot that handles setting the soft input 2 control.
	void onSoftInput2Toggled(bool state);
	/// Slot that handles updating the checked state of the soft input 2 button.
	void updateSoftInput2();
	/// Slot that handles setting the soft input 3 control.
	void onSoftInput3Toggled(bool state);
	/// Slot that handles updating the checked state of the soft input 3 button.
	void updateSoftInput3();
	/// Slot that handles setting the soft input 4 control.
	void onSoftInput4Toggled(bool state);
	/// Slot that handles updating the checked state of the soft input 4 button.
	void updateSoftInput4();

protected:
	/// The zebra model.
	BioXASZebra *zebra_;

	/// The soft input 1 button.
	QPushButton *softInput1Button_;
	/// The soft input 2 button.
	QPushButton *softInput2Button_;
	/// The soft input 3 button.
	QPushButton *softInput3Button_;
	/// The soft input 4 button.
	QPushButton *softInput4Button_;
};

#endif // BIOXASZEBRAVIEW_H
