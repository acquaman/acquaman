#ifndef AMSAMPLEETHAN_H
#define AMSAMPLEETHAN_H

#include "dataman/database/AMDbObject.h"
#include "util/AMElement.h"
#include "dataman/AMScan.h"
#include "dataman/AMSamplePlate.h"
#include "dataman/database/AMDbObjectSupport.h"
#include "source/acquaman.h"



class AMSampleEthan : public AMDbObject
{
    Q_OBJECT
    Q_PROPERTY(QDateTime dateTime READ dateTime WRITE setDateTime)
    Q_PROPERTY(QString notes READ notes WRITE setNotes)
    Q_PROPERTY(QByteArray image READ rawImage WRITE setRawImage)
    Q_PROPERTY(QStringList tags READ tags WRITE setTags)
    Q_PROPERTY(AMIntList elementList READ elementList WRITE setElementList)

    Q_CLASSINFO("AMDbObject_Attributes", "doNotReuseIds=true;description=Sample")
    Q_CLASSINFO("elementIds", "hidden=true")
public:
    /// default constructor
    explicit AMSampleEthan(QObject* parent = 0);
    /// This constructor initializes a sample with a given name.
    AMSampleEthan(const QString& sampleName, QObject* parent = 0);
    /// this constructor immediately loads a stored sample from the database.
    AMSampleEthan(int databaseId, AMDatabase *database, QObject *parent = 0);


    /// sample management interface
    void destroySample();
    static void destroySample(AMDatabase* db, int sampleId);




    /// accessors
    QDateTime dateTime() const;
    QString dateTimeString() const;
    QString notes() const;
    QByteArray image() const;
    QList<const AMElement*> elements() const;
    QList<AMScan*> scanList() const;
    QStringList tags() const;
    AMSamplePlate* samplePlate() const;
    QList<int> elementList() const;
    QList<int> sampleNumber() const;


    /// thumbnails
    int thumbnailCount() const;
    AMDbThumbnail thumbnail(int index) const;
    QByteArray rawImage() const;



    /// returns true if string is in tags_, else returns false
    bool hasTag(QString tag) const;
    /// returns true if scan is in scanList_, else returns false
    bool hasScan(AMScan* scan) const;
    /// returns a string list of the element symbols
    QString elementString() const;


public slots:

    /// mutators
    void setDateTime(const QDateTime dateTime);
    void setNotes(const QString notes);
    void setImage(const QImage& image);
    void setRawImage(const QByteArray& rawPngImage);
    void setElements(const QList<const AMElement*> elements);
    void setScanList(const QList<AMScan*> scanList);
    void setTags(const QStringList tags);
    void setSamplePlate(AMSamplePlate* samplePlate);
    void setElementList(const QList<int>& elements);
    void setSampleNumber(const QList<int> sampleNumber);

    /// adds a tag to the stringList, if it is not already in it
    void addTag(const QString tag);
    /// removes all instances of a tag, if it is in the tag list, else does nothing
    void removeTag(const QString tag);

    /// adds a scan to the scanList, if it is not already in it
    void addScan(AMScan* scan);
    /// removes all instances of a scan, if it is in the scanList, else does nothing
    void removeScan(AMScan* scan);

    void addElement(const AMElement* element);
    void removeElement(const AMElement* element);
    void toggleElement(const AMElement* element);

    void setCurrentDateTime();

protected:

    QDateTime dateTime_;
    QString notes_;
    QByteArray image_;
    QList<const AMElement*> elements_;
    QList<AMScan*> scanList_;
    QStringList tags_;
    AMSamplePlate* samplePlate_;


};

#endif // AMSAMPLEETHAN_H
