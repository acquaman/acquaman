#ifndef AMSORTFILTERWIDGET_H
#define AMSORTFILTERWIDGET_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QApplication>
#include <QGridLayout>
#include <QFormLayout>

/**
 * A class representing the widget area used to build a filter. Has widgets for setting the field and
 * the filter type (contains, exact match, regex). Note: This doesn't apply filters to anything, it is simply
 * a way to format the possible criteria in a handy widget. If you want an actual widget to do some filtering
 * of a QSortFilterProxyModel, see AMSortFilterWidget below.
 */
class AMSortFilterBuilder : public QWidget {
	Q_OBJECT

public:
	explicit AMSortFilterBuilder(QWidget* parent = 0);
	/// Returns the currently selected filter column
	int filterKeyColumn() const;
	/// Returns a criteria text formatted depending on the current mode selection
	QString criteriaText() const;
	/// Returns whether the filter is case sensitiver or not
	bool isCaseSensitive() const;

	/// Adds a field to the field combo box
	void addField(const QString& fieldName);
	/// Clears all fields from the field combo box
	void clearFields();
	/// Returns the form layout used to hold the FilterBuilder's Widgets
	QFormLayout* formLayout();
public slots:
	/// Clears the search criteria
	void clear();
private:
	QComboBox* fieldComboBox_;
	QComboBox* modeComboBox_;
	QLineEdit* criteriaLineEdit_;
	QCheckBox* caseSensitiveCheckBox_;
	QFormLayout* layout_;
};

/**
 * A class representing a standardized widget for controlling any QSortFilterProxyModel. When passed
 * a QSortFilterProxyModel during instantiation, the widget will automatically build the controls featuring
 * the fields currently known to the QSortFilterProxyModel.
 */
class AMSortFilterWidget : public QWidget
{
	Q_OBJECT

public:
	/// Creates an instance of an AMSortFilterWidget for controlling the provided QSortFilterProxyModel
	explicit AMSortFilterWidget(QSortFilterProxyModel* model, QWidget *parent = 0);
	/// Whether or not the ProxyModel currently has a filter applied
	bool isCurrentlyFiltered();
	/// Add an additional search column. This can be used to search columns in hierarchical models which
	/// aren't in the top level, and so aren't automatically added to the widget. Caution, this may require
	/// the proxy model to be customized in a way to accept such criteria.
	void addManualColumn(const QString& header);
signals:
	/// Emitted whenever the filter the Widget is applying is altered
	void filterChanged(bool);
protected slots:
	/// Slot to handle requests to apply the filter, either through clicking the button or pressing return
	/// when in the criteria line edit
	void onFilterApplied();
	/// Slot to handle the clear button being clicked
	void onClearFilterButtonClicked();
	/// Sets the visibility state of the main filter layout area
	void setFilterAreaVisible(bool visible);
	/// The sort filter proxy model this widget is controlling
	QSortFilterProxyModel* model();
protected:
	/// The model which the AMSortFilterWidget is controlling
	QSortFilterProxyModel* proxyModel();
	/// Refreshes the fieldComboBox_ widget with the columns currently known to the QSortFilterProxyModel
	void refreshColumns();
	/// Places the child widgets in layouts and adds those layouts to the AMSortFilterWidget
	void initLayout();	
	/// Initializes the apply and clean push buttons
	void initPushButtons();
	/// Handles changes between a filter being applied to the data and not
	void setCurrentlyFiltered(bool value);
	QPushButton* toggleHideButton_;
	QLabel* filteredWarningLabel_;
	QPushButton* clearFilterPushButton_;
	QPushButton* applyFilterPushButton_;
	QSortFilterProxyModel* proxyModel_;
	AMSortFilterBuilder* filterBuilder_;
	bool isCurrentlyFiltered_;
private:


};



#endif // AMSORTFILTERWIDGET_H
