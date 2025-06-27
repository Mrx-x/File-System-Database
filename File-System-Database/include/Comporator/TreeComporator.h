#ifndef TREECOMPORATOR_H
#define TREECOMPORATOR_H

#include <QString>
#include <vector>

#include "Model/ScanItem.h"

struct ChangeEntry
{
    QString path;
    ScanItem::Change type;
};

class TreeComporator
{
public:
    static std::vector<ChangeEntry> compare(const ScanItem* oldRoot, ScanItem* newRoot);
};

#endif // TREECOMPORATOR_H
