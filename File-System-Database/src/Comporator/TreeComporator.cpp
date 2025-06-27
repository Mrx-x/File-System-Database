#include "Comporator/TreeComporator.h"

#include <QHash>
#include <QDir>

void collectOldNodes(const ScanItem* node, const QString& prefix, QHash<QString, qint64>& outMap)
{
    QString fullPath = prefix.isEmpty() ? node->name()
                                        : prefix + QDir::separator() + node->name();

    outMap.insert(fullPath, node->size());
    for (int i = 0; i < node->childCount(); ++i)
    {
        collectOldNodes(node->child(i), fullPath, outMap);
    }
}

void compareNewNodes(QHash<QString, qint64>& oldMap, ScanItem* node, const QString& prefix, std::vector<ChangeEntry>& changes)
{
    QString fullPath = prefix.isEmpty() ? node->name()
                                        : prefix + QDir::separator() + node->name();

    if (!oldMap.contains(fullPath))
    {
        node->setChange(ScanItem::Change::New);
        changes.push_back({ fullPath, ScanItem::Change::New });
    }
    else
    {
        qint64 oldSize = oldMap.value(fullPath);
        if (node->size() != oldSize)
        {
            node->setChange(ScanItem::Change::Modified);
            changes.push_back({ fullPath, ScanItem::Change::Modified });
        }
        else
        {
            node->setChange(ScanItem::Change::Unchanged);
        }
        oldMap.remove(fullPath);
    }

    for (int i = 0; i < node->childCount(); ++i)
    {
        compareNewNodes(oldMap, node->child(i), fullPath, changes);
    }
}

std::vector<ChangeEntry> TreeComporator::compare(const ScanItem* oldRoot, ScanItem* newRoot)
{
    std::vector<ChangeEntry> changes;
    QHash<QString, qint64> oldMap;

    collectOldNodes(oldRoot, QString(), oldMap);
    compareNewNodes(oldMap, newRoot, QString(), changes);

    for (auto it = oldMap.constBegin(); it != oldMap.constEnd(); ++it)
    {
        changes.push_back({ it.key(), ScanItem::Change::Deleted });
    }

    return changes;
}
