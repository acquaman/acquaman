#ifndef AMSORTFILTERWIDGET_H
#define AMSORTFILTERWIDGET_H

#include <QWidget>
#include <QSortFilterProxyModel>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>


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
public:
	explicit AMSortFilterWidget(QSortFilterProxyModel* model, QWidget *parent = 0);
protected:
	QSortFilterProxyModel* proxyModel();
	void refreshColumns();
	void initLayout();
	void initModes();
	void initCaseSensitivity();
	void initPushButtons();
	void setCurrentlyFiltered(bool value);
signals:
	
protected slots:
	void onFilterApplied();
	void onClearFilterButtonClicked();
public slots:
	
};

#endif // AMSORTFILTERWIDGET_H
