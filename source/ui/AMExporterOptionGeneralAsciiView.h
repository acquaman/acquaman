#ifndef AMEXPORTEROPTIONGENERALASCIIVIEW_H
#define AMEXPORTEROPTIONGENERALASCIIVIEW_H

#include <QTabWidget>
#include "ui_AMExporterOptionGeneralAsciiView.h"

class AMExporterOptionGeneralAscii;

class AMExporterOptionGeneralAsciiView : public QTabWidget, private Ui::AMExporterOptionGeneralAsciiView
{
	Q_OBJECT

public:
	explicit AMExporterOptionGeneralAsciiView(AMExporterOptionGeneralAscii* option, QWidget *parent = 0);
	virtual ~AMExporterOptionGeneralAsciiView();


};

#endif // AMEXPORTEROPTIONGENERALASCIIVIEW_H
