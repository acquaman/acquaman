#ifndef BIOXASCONTROLEDITORVALUEVIEW_H
#define BIOXASCONTROLEDITORVALUEVIEW_H

#include <QWidget>
#include <QLabel>
#include <QStyleOption>
#include <QPaintEvent>
#include <QApplication>
#include <QPainter>

class AMControl;

class BioXASControlEditorValueView : public QWidget
{
    Q_OBJECT

public:
	/// Constructor.
	explicit BioXASControlEditorValueView(AMControl *control, QWidget *parent = 0);
	/// Destructor.
	virtual ~BioXASControlEditorValueView();

	/// Returns the control being viewed.
	AMControl* control() const { return control_; }

signals:
	/// Notifier that the control being viewed has changed.
	void controlChanged(AMControl *newControl);

public slots:
	/// Sets the control being viewed.
	void setControl(AMControl *newControl);

protected:
	/// Paints the value label. Reimplemented to add progress bar functionality.
	virtual void paintEvent(QPaintEvent *event);

protected:
	/// The control being viewed.
	AMControl *control_;

	/// The label.
	QLabel *label_;
};

#endif // BIOXASCONTROLEDITORVALUEVIEW_H
