#ifndef AMSAMPLEPLATEBROWSERVIEW_H
#define AMSAMPLEPLATEBROWSERVIEW_H

#include <QWidget>
#include <QDialog>
#include <QModelIndex>

class QListView;
class QLineEdit;
class QLabel;
class QPushButton;

class AMSamplePlate;
class AMSamplePlateBrowser;

class AMSamplePlateBrowserView : public QWidget
{
Q_OBJECT
public:
	AMSamplePlateBrowserView(AMSamplePlateBrowser *samplePlateBrowser, QWidget *parent = 0);

public slots:
	void clearViewSelection();

signals:
	void samplePlateSelected(AMSamplePlate *samplePlate);

protected slots:
	void onSelectButtonClicked();

	void onSelectionModelCurrentChanged(const QModelIndex &current, const QModelIndex &previous);

	void onSamplePlatesListViewDoubleClicked(const QModelIndex &clicked);

protected:
	void keyPressEvent(QKeyEvent *event);

protected:
	AMSamplePlateBrowser *samplePlateBrowser_;
	QListView *samplePlatesListView_;
	QLabel *selectionLabel_;
	QPushButton *selectButton_;
	QPushButton *cancelButton_;
};

class AMSamplePlateCreationDialog : public QDialog
{
Q_OBJECT
public:
	AMSamplePlateCreationDialog(QWidget *parent = 0);

	QString samplePlateName() const;

protected slots:
	void onSamplePlateNameLineEditTextChanged(const QString &name);

protected:
	QLineEdit *samplePlateNameLineEdit_;
	QPushButton *okButton_;
	QPushButton *cancelButton_;
};

#endif // AMSAMPLEPLATEBROWSERVIEW_H
