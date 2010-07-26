#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>

class ComboBox: public QComboBox
{
	Q_Object

public:
	ComboBox (QWidget *parent = 0);
	QList<string> rv;
	QList<string> rv1;
	QList<string> rv2;

private slots:

};

#endif // COMBOBOX_H
