#ifndef STRIPTOOLABSTRACTVARIABLEINFO_H
#define STRIPTOOLABSTRACTVARIABLEINFO_H

#include <QObject>

class StripToolAbstractVariableInfo : public QObject
{
    Q_OBJECT

    friend class StripToolAbstractVariable;
    friend class StripToolVariableInfoEditor;

public:
    explicit StripToolAbstractVariableInfo(QObject *parent = 0);

signals:
    void infoChanged();

protected:
    virtual bool hasName() const;
    virtual bool hasCreationDateTime() const;
    virtual bool hasDescription() const;
    virtual bool hasUnits() const;
    virtual bool hasGranularity() const;
    virtual bool hasColor() const;
    virtual bool hasSelectionState() const;
    virtual bool hasCheckState() const;
    virtual bool hasTimeUnits() const;

    QString name() const;
    void setName(const QString &sourceName);

    QString description() const;
    void setDescription(const QString &newDescription);

protected:
    bool hasName_;
    bool hasCreationDateTime_;
    bool hasDescription_;
    bool hasUnits_;
    bool hasGranularity_;
    bool hasColor_;
    bool hasSelectionState_;
    bool hasCheckState_;
    bool hasTimeUnits_;

private:
    QString name_;
    QString description_;

};

#endif // STRIPTOOLABSTRACTVARIABLEINFO_H
