#ifndef STPLOTEDITOR_H
#define STPLOTEDITOR_H

#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLayout>

class STPlotEditor : public QWidget
{
    Q_OBJECT

public:
    /// Constructor.
    explicit STPlotEditor(QWidget *parent = 0);
    /// Destructor.
    ~STPlotEditor();

    /// Returns the timeEntry widget.
    QLineEdit* timeEntry() const;
    /// Returns the timeUnits widget.
    QComboBox* timeUnits() const;

signals:
    /// Emitted when the OK button is clicked.
    void editComplete();
    /// Emitted when the Cancel button is clicked.
    void editCancelled();

public slots:
    /// Sets the value to appear in the timeEntry widget.
    void setTimeEntry(int timeValue);
    /// Sets the current time units. Not implemented yet.
    void setTimeUnits();

protected:
    QLineEdit *timeEntry_;
    QComboBox *timeUnits_;
    QPushButton *okButton_;
    QPushButton *cancelButton_;


};

#endif // STPLOTEDITOR_H
