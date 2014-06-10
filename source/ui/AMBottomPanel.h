#ifndef AMBOTTOMPANEL_H
#define AMBOTTOMPANEL_H

#include <QFrame>
#include <QHBoxLayout>

/// This is a base class for the bottom widget of the AM main window.  It has no features except for defining the standard stylesheet and a way to change it.
class AMBottomPanel : public QFrame
{
	Q_OBJECT

public:
	/// Constructor.  Builds the widget in the current standard style.
 	virtual ~AMBottomPanel();
	AMBottomPanel(QWidget *parent = 0);

protected:
	/// Returns the layout of the panel.
	QHBoxLayout *panelLayout() const { return layout_; }

	/// The pointer to the layout of the panel.
	QHBoxLayout *layout_;
};

#endif // AMBOTTOMPANEL_H
