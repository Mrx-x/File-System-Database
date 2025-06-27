#include "Model/ScanItem.h"

ScanItem::ScanItem(const QString& name, qint64 size, Type type, ScanItem* parent)
    : _parent(parent)
    , _name(name)
    , _size(size)
    , _type(type)
{

}

ScanItem::~ScanItem()
{

}

void ScanItem::appendChild(std::unique_ptr<ScanItem> child)
{
    _children.push_back(std::move(child));
}

ScanItem* ScanItem::child(int idx) const
{
    if (idx < 0 || idx >= _children.size()) return nullptr;
    return _children[idx].get();
}

int ScanItem::childCount() const
{
    return _children.size();
}

ScanItem* ScanItem::parent() const
{
    return _parent;
}

QString ScanItem::name() const
{
    return _name;
}

qint64 ScanItem::size() const
{
    return _size;
}

void ScanItem::setSize(qint64 size)
{
    _size = size;
}

ScanItem::Type ScanItem::type() const
{
    return _type;
}

