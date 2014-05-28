//#ifndef STRIPTOOLABSTRACTVARIABLEINFO_H
//#define STRIPTOOLABSTRACTVARIABLEINFO_H

//#include <QObject>

//class StripToolAbstractVariableInfo : public QObject
//{
//    Q_OBJECT

//    friend class StripToolAbstractVariable;
//    friend class StripToolModel;
//    friend class StripToolVariableInfoEditor;
//    friend class StripToolVariableInfoImporter;
//    friend class StripToolVariableInfoExporter;

//public:
//    explicit StripToolAbstractVariableInfo(QObject *parent = 0);

//signals:
//    void infoChanged();

//public:
//    virtual bool hasName() const;
//    virtual bool hasCreationDateTime() const;
//    virtual bool hasDescription() const;
//    virtual bool hasUnits() const;
//    virtual bool hasGranularity() const;
//    virtual bool hasColor() const;
//    virtual bool hasSelectionState() const;
//    virtual bool hasCheckState() const;
//    virtual bool hasTimeUnits() const;

//    QString name() const;
//    QString description() const;

//public slots:
//    void setName(const QString &sourceName);
//    void setDescription(const QString &newDescription);

//private:
//    QString name_;
//    QString description_;

//    bool hasName_;
//    bool hasCreationDateTime_;
//    bool hasDescription_;
//    bool hasUnits_;
//    bool hasGranularity_;
//    bool hasColor_;
//    bool hasSelectionState_;
//    bool hasCheckState_;
//    bool hasTimeUnits_;

//};

//#endif // STRIPTOOLABSTRACTVARIABLEINFO_H
