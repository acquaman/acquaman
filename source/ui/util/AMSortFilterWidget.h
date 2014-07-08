#ifndef AMSORTFILTERWIDGET_H
#define AMSORTFILTERWIDGET_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

/**
 * A class representing a standardized widget for controlling any QSortFilterProxyModel. When passed
 * a QSortFilterProxyModel during instantiation, the widget will automatically build the controls featuring
 * the fields currently known to the QSortFilterProxyModel.
 */
class AMSortFilterWidget : public QWidget
{
	Q_OBJECT
private:
	QComboBox* fieldComboBox_;
	QComboBox* modeComboBox_;
	QLineEdit* criteriaLineEdit_;
	QCheckBox* caseSensitiveCheckBox_;
	QPushButton* clearFilterPushButton_;
	QPushButton* applyFilterPushButton_;
	QSortFilterProxyModel* proxyModel_;
	bool isCurrentlyFiltered_;
public:
	/// Creates an instance of an AMSortFilterWidget for controlling the provided QSortFilterProxyModel
	explicit AMSortFilterWidget(QSortFilterProxyModel* model, QWidget *parent = 0);
	/// Whether or not the ProxyModel currently has a filter applied
	bool isCurrentlyFiltered();
protected:
	/// The model which the AMSortFilterWidget is controlling
	QSortFilterProxyModel* proxyModel();
	/// Refreshes the fieldComboBox_ widget with the columns currently known to the QSortFilterProxyModel
	void refreshColumns();
	/// Places the child widgets in layouts and adds those layouts to the AMSortFilterWidget
	void initLayout();	
	/// Populates the modeComboBox_ with the modes of filtering (Contains, Exact Match, Regular Expression)
	void initModes();
	/// Sets the checked status of the caseSensitiveCheckBox_ to match the status of the QSortFilterProxyModel
	void initCaseSensitivity();
	/// Initializes the apply and clean push buttons
	void initPushButtons();
	/// Handles changes between a filter being applied to the data and not
	void setCurrentlyFiltered(bool value);

signals:
	/// Emitted whenever the widget changes from filtering to not filtering
	void isCurrentlyFilteredStateChanged(bool);
protected slots:
	/// Slot to handle requests to apply the filter, either through clicking the button or pressing return
	/// when in the criteria line edit
	void onFilterApplied();
	/// Slot to handle the clear button being clicked
	void onClearFilterButtonClicked();
public slots:
	
};

#endif // AMSORTFILTERWIDGET_H
