#ifndef REIXSXESHEXAPODCONTROLEDITOR_H
#define REIXSXESHEXAPODCONTROLEDITOR_H

#include <QGroupBox>

class REIXSHexapod;

class REIXSXESHexapodControlEditor : public QGroupBox
{
	Q_OBJECT
public:
	explicit REIXSXESHexapodControlEditor(REIXSHexapod* hexapod, QWidget *parent = 0);

signals:

public slots:

protected:
	REIXSHexapod* hexapod_;


};

#endif // REIXSXESHEXAPODCONTROLEDITOR_H
