#ifndef AMNUMBERCHANGEACTIONEDITOR_H
#define AMNUMBERCHANGEACTIONEDITOR_H

#include <QFrame>
#include "actions3/actions/AMNumberChangeActionInfo.h"

class QSpinBox;
class QLabel;

class AMNumberChangeActionEditor : public QFrame
{
Q_OBJECT
public:
	Q_INVOKABLE AMNumberChangeActionEditor(AMNumberChangeActionInfo *info, QWidget *parent = 0);

protected slots:
	void onIndexSpinBoxEditingFinished();
	void onNewNumberSpinBoxEditingFinished();

protected:
	AMNumberChangeActionInfo *info_;

	QSpinBox *indexSpinBox_;
	QSpinBox *newNumberSpinBox_;
	QLabel *descriptionLabel_;
};

#endif // AMNUMBERCHANGEACTIONEDITOR_H
