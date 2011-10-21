#ifndef AMDICTIONARYLINEEDIT_H
#define AMDICTIONARYLINEEDIT_H

#include <QLineEdit>

class QLabel;
class AMOldScanParametersDictionary;

class AMDictionaryLineEdit : public QLineEdit
{
Q_OBJECT
public:
	AMDictionaryLineEdit(AMOldScanParametersDictionary *dictionary, QLabel *exampleLabel = 0, QWidget *parent = 0);

public slots:
	void setTextAndOperate(const QString &text);

protected slots:
	void onTextEdited(const QString &text);

protected:
	void keyPressEvent(QKeyEvent *);

protected:
	AMOldScanParametersDictionary *dictionary_;
	QLabel *exampleLabel_;
};

#endif // AMDICTIONARYLINEEDIT_H
