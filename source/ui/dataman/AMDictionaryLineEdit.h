#ifndef AMDICTIONARYLINEEDIT_H
#define AMDICTIONARYLINEEDIT_H

#include <QLineEdit>

class QLabel;
class AMScanParametersDictionary;

class AMDictionaryLineEdit : public QLineEdit
{
Q_OBJECT
public:
	AMDictionaryLineEdit(AMScanParametersDictionary *dictionary, QLabel *exampleLabel = 0, QWidget *parent = 0);

public slots:
	void setTextAndOperate(const QString &text);
	void operate();

signals:
	void operated();

protected slots:
	void onTextEdited(const QString &text);

protected:
	void keyPressEvent(QKeyEvent *);

protected:
	AMScanParametersDictionary *dictionary_;
	QLabel *exampleLabel_;
};

#endif // AMDICTIONARYLINEEDIT_H
