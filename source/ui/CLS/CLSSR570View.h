//#ifndef CLSSR570VIEW_H
//#define CLSSR570VIEW_H

//#include <QWidget>

//#include <QComboBox>
//#include <QToolButton>

//#include "beamline/CLS/CLSSR570.h"

///// This simple widget provides straight forward access to changing the sensitivity on the SR570.
//class CLSSR570View : public QWidget
//{
//	Q_OBJECT

//public:
//	/// Simple enum holding whether the view should be either basic or advanced looking.
//	enum ViewMode { Basic = 0, Advanced = 1 };

//	/// Constructor.  Takes in a valid SR570 pointer.
//	explicit CLSSR570View(CLSSR570 *sr570, QWidget *parent = 0);
//    virtual ~CLSSR570View();

//	/// Returns the pointer to the SR570 this view is visualizing.
//	CLSSR570 *sr570() const { return sr570_; }

//	/// Returns the current view mode.
//	CLSSR570View::ViewMode viewMode() const { return mode_; }
//	/// Switches the view from basic to advanced or advanced to basic.
//	void setViewMode(ViewMode mode);

//signals:
//	/// Notifier that the view has changed.  Passes the new view.
//	void viewModeChanged(CLSSR570View::ViewMode);

//public slots:

//protected slots:
//	/// Handles passing changes in the value combo box to the ion chamber.
//	void onValueComboBoxChanged(int index);
//	/// Handles passing changes in the units combo box to the ion chamber.
//	void onUnitsComboBoxChanged(int index);

//	/// Handles setting the value combo box when the ion chamber is changed from elsewhere.
//	void onValueChanged(int value);
//	/// Handles setting the units combo box when the ion chamber is changed from elsewhere.
//	void onUnitsChanged(QString units);

//	/// Builds a popup menu for switching view modes.
//	virtual void onCustomContextMenuRequested(QPoint pos);

//protected:
//	/// The pointer to the SR570 this view manages.
//	CLSSR570 *sr570_;

//	/// Combo box holding the value for the sensitivity.
//	QComboBox *value_;
//	/// Combo box holding the units for the sensitivity.
//	QComboBox *units_;

//	/// The tool button for the minus button.
//	QToolButton *minus_;
//	/// The tool button for the plus button.
//	QToolButton *plus_;

//	/// Flag holding whether or not the view is basic or advanced.
//	ViewMode mode_;
//};

//#endif // CLSSR570VIEW_H
