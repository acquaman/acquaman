#ifndef AMSAMPLEETHANVIEW_H
#define AMSAMPLEETHANVIEW_H

#include <QWidget>

class QLineEdit;
class AMSample;
class QPushButton;
class QCompleter;
class QStringListModel;
class QComboBox;
class QTextEdit;
class AMElementListEdit;
class AMPeriodicTableDialog;
class AMShapeDataView;

/// This class is the view for an AMSampleEthan.
/// It can be used to load or save a sample to the database,
/// as well as to set or view the name, date, notes, tags, elements,
/// and sample plate of the sample.
class AMSampleView : public QWidget
{
    Q_OBJECT
public:
    AMSampleView(QWidget* parent = 0);
    AMSampleView(AMSample* sample, QWidget* parent = 0);

public slots:
    /// set the sample to view
    void setSample(AMSample* sample);
    /// set the name of the current sample
    void setName(QString name);
    /// set the dateTime of the current sample
    void setDateTime(QString dateTime);
    /// set the notes of the current sample
    void setNotes();
    /// add a tag to the current sample
    void addTag();
    /// remove a tag from a specified sample
    void removeTag(int index);
    /// remove a tag from the current sample
    void removeTag();
    /// show the periodic table dialog box
    void showPeriodicTable();


protected slots:
    /// update the fields in the view
    void updateFrames();
    /// save the sample to the database
    void saveToDb();
    /// load a sample from the database
    void loadSample(QString);
    /// change the sample plate associated with this sample
    void changeSamplePlate(QString);

protected:
    /// set up the ui elements of the view
    void setUpGui();
    /// make the views connections
    void makeConnections();
    /// populate the sample plate combobox
    void populateSamplePlateLoader();
    /// load the sample from the database
    void loadFromDb();
    /// populate the sample combo box.
    void populateSampleLoader();



private:
    /// text box current tag
    QLineEdit* tagText_;
    /// text box showing name
    QLineEdit* nameText_;
    /// text box showing dateTime
    QLineEdit* dateTimeText_;
    /// text box showing notes
    QTextEdit* notesText_;
    /// text box showing elements
    QLineEdit* elementsText_;
    /// push button to bring up th element dialog
    QPushButton* showElementDialog_;
    /// the element dialog window
    AMPeriodicTableDialog* elementDialog_;

    /// the combobox showing all the current samples tags
    QComboBox* tagBox_;
    /// completer for autocompleting tags
    QCompleter* completer_;
    /// list of all previously used tags, used for autocomplete
    QStringListModel* wordList_;
    /// push button used to remove tags from the current sample
    QPushButton* removeTagButton_;

    /// text box showing the sample plate name
    QLineEdit* samplePlateName_;
    /// combo box used to load a sample plate from the database
    QComboBox* samplePlateLoader_;

    /// combo box used to load a sample from the database
    QComboBox* sampleLoader_;
    /// push button used to save the current sample to the database
    QPushButton* saveToDb_;

    /// the sample that is being viewed by this view
    AMSample* sample_;

    /// view for visualizing the sample's AMShapeData
    AMShapeDataView* shapeDataView_;
};

#endif // AMSAMPLEETHANVIEW_H
