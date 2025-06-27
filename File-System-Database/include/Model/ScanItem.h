#ifndef SCANITEM_H
#define SCANITEM_H

#include <QString>
#include <vector>

class ScanItem
{
public:
    enum class Type { File, Folder };

    ScanItem(const QString& name, qint64 size, Type type, ScanItem* parent = nullptr);
    ~ScanItem();

public:
    void appendChild(std::unique_ptr<ScanItem> child);
    ScanItem* child(int idx) const;
    ScanItem* parent() const;
    int childCount() const;

public:
    qint64 size() const;
    void setSize(qint64 size);
public:
    QString name() const;
    Type type() const;

private:
    std::vector<std::unique_ptr<ScanItem>> _children;
    ScanItem* _parent;
    QString _name;
    qint64 _size;
    Type _type;
};

#endif // SCANITEM_H
