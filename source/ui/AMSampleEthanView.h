#ifndef AMSAMPLEETHANVIEW_H
#define AMSAMPLEETHANVIEW_H

#include <QWidget>

class QLineEdit;
class AMSampleEthan;
class QPushButton;
class QCompleter;
class QStringListModel;
class QComboBox;
class QTextEdit;
class AMElementListEdit;
class AMPeriodicTableDialog;

class AMSampleEthanView : public QWidget
{
    Q_OBJECT
public:
    AMSampleEthanView(QWidget* parent = 0);
    AMSampleEthanView(AMSampleEthan* sample, QWidget* parent = 0);

public slots:
    void setSample(AMSampleEthan* sample);
    void setName(QString name);
    void setDateTime(QString dateTime);
    void setNotes();
    void addTag();
    void showPeriodicTable();

protected slots:
    void update();
    void saveToDb();
    void loadSample(QString);

protected:
    void setUpGui();
    void makeConnections();
    void loadFromDb();
    void populateSampleLoader();


private:
    QLineEdit* tagText_;
    QLineEdit* nameText_;
    QLineEdit* dateTimeText_;
    QTextEdit* notesText_;
    QLineEdit* elementsText_;
    AMElementListEdit* elementList_;
    QPushButton* showElementDialog_;
    AMPeriodicTableDialog* elementDialog_;

    QComboBox* tagBox_;
    QCompleter* completer_;
    QStringListModel* wordList_;

    QComboBox* sampleLoader_;
    QPushButton* saveToDb_;

    AMSampleEthan* sample_;
};

#endif // AMSAMPLEETHANVIEW_H
