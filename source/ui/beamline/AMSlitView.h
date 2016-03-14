#ifndef AMSLITVIEW_H
#define AMSLITVIEW_H

#include <QWidget>
#include <QPushButton>
#include <QGroupBox>
#include <QLayout>

class AMSlit;
class AMExtendedControlEditor;

class AMSlitView : public QWidget
{
	Q_OBJECT

public:
	/// Constructor.
	explicit AMSlitView(AMSlit *slit, bool showBladeEditors = false, QWidget *parent = 0);
	/// Destructor.
	virtual ~AMSlitView();

	/// Returns the slit being viewed.
	AMSlit* control() const { return slit_; }

signals:
	/// Notifier that the slit being viewed has changed.
	void controlChanged(AMSlit *newControl);

public slots:
	/// Refreshes the view.
	void refresh();
	/// Sets the slit being viewed.
	void setControl(AMSlit *newControl);
	/// Sets the flag for whether the blade editors are shown.
	void setBladeEditorsShown(bool showEditors);

protected slots:
	/// Updates the gap editor.
	void updateGapEditor();
	/// Updates the center editor.
	void updateCenterEditor();
	/// Updates the open button.
	void updateOpenButton();
	/// Updates the closed button.
	void updateClosedButton();
	/// Updates the blade editors.
	void updateBladeEditors();

	/// Handles opening the slit when the open button is clicked.
	void onOpenButtonClicked();
	/// Handles closing the slit when the close button is clicked.
	void onCloseButtonClicked();

protected:
	/// The slit being viewed.
	AMSlit *slit_;

	/// The slit gap editor.
	AMExtendedControlEditor *gapEditor_;
	/// The slit center editor.
	AMExtendedControlEditor *centerEditor_;
	/// The open button.
	QPushButton *openButton_;
	/// The close button.
	QPushButton *closeButton_;

	/// Flag indicating whether the blade editors should be shown.
	bool showBladeEditors_;
	/// The blade editors group.
	QGroupBox *bladesBox_;
	/// The first blade editor.
	AMExtendedControlEditor *firstBladeEditor_;
	/// The second blade editor.
	AMExtendedControlEditor *secondBladeEditor_;
};

#endif // AMSLITVIEW_H
