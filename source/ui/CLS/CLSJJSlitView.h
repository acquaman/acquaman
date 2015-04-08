#ifndef CLSJJSLITVIEW_H
#define CLSJJSLITVIEW_H

#include <QWidget>

class AMExtendedControlEditor;
class CLSJJSlit;

class CLSJJSlitView : public QWidget
{
    Q_OBJECT
public:
	explicit CLSJJSlitView(CLSJJSlit *clsJJSlit, QWidget *parent = 0);

signals:

public slots:

protected:
	CLSJJSlit * clsJJSlit_;

	AMExtendedControlEditor *verticalGapEditor_;
	AMExtendedControlEditor *verticalCenterEditor_;

	AMExtendedControlEditor *horizontalGapEditor_;
	AMExtendedControlEditor *horizontalCenterEditor_;

};

#endif // CLSJJSLITVIEW_H
