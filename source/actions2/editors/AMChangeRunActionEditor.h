#ifndef AMCHANGERUNACTIONEDITOR_H
#define AMCHANGERUNACTIONEDITOR_H

#include <QFrame>
#include "actions2/actions/AMChangeRunActionInfo.h"

class AMRunSelector;
class QLineEdit;

/// Provides in-place editor for an action to change the current run.
class AMChangeRunActionEditor : public QFrame
{
    Q_OBJECT
public:
	/// Constructor must accept a AMChangeRunActionInfo as argument
	Q_INVOKABLE AMChangeRunActionEditor(AMChangeRunActionInfo* info, QWidget *parent = 0);

signals:

public slots:

protected slots:
	/// Called when the radio button is toggled to switch between existing run / new run.
	void onNewRunRadioButtonToggled(bool newRun);
	/// Called when the current run is changed in the selector
	void onExistingRunChanged(int);
	/// Called when the name of the new run is changed
	void onNewRunNameChanged(const QString& newRunName);

protected:
	AMRunSelector* runSelector_;
	QLineEdit* newNameEdit_;
	AMChangeRunActionInfo* info_;

};

#endif // AMCHANGERUNACTIONEDITOR_H
