#include "Scanner/DirectoryScanner.h"
#include "Model/ScanItem.h"

#include <QDir>
#include <QFileInfoList>

DirectoryScanner::DirectoryScanner(QObject* parent)
    : IScanner(parent)
{

}

std::unique_ptr<ScanItem> DirectoryScanner::scan(const QString& path) const
{
    auto root = scanFolder(path, nullptr);
    return root;
}

std::unique_ptr<ScanItem> DirectoryScanner::scanFolder(const QString &path, ScanItem *parent) const
{
    QDir dir(path);
    auto folderItem = std::make_unique<ScanItem>(dir.dirName(), 0, ScanItem::Type::Folder, parent);

    qint64 totalSize = 0;

    const QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);

    for (const QFileInfo& entry : entries)
    {
        if (entry.isDir())
        {
            auto child = scanFolder(entry.absoluteFilePath(), folderItem.get());
            totalSize += child->size();
            folderItem->appendChild(std::move(child));
        }
        else
        {
            qint64 fileSize = entry.size();
            auto fileItem = std::make_unique<ScanItem>(entry.fileName(), fileSize, ScanItem::Type::File, folderItem.get());
            totalSize += fileSize;
            folderItem->appendChild(std::move(fileItem));
        }
    }

    folderItem->setSize(totalSize);
    return folderItem;
}
