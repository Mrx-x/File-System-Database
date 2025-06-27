#include "Controller/ScanController.h"
#include "Database/IDatabaseService.h"

#include <QStandardItem>
#include <QList>
#include <QLocale>
#include <QDateTime>
#include <QInputDialog>
#include <QApplication>
#include <QStyle>

ScanController::ScanController(IScanner *scanner, const DatabasePtr& database, QObject *parent)
    : QObject(parent)
    , _scanner(scanner)
    , _database(database)
{
    _model = new QStandardItemModel(this);
    clearModel();
}

void ScanController::doScan(const QString &path)
{
    _lastScanItem = _scanner->scan(path);
    _lastPath = path;
    _lastTime = QDateTime::currentDateTime();
    buildModel(_lastScanItem.get());

    auto prevScan = _database->loadLastScanByPath(path);

    if (prevScan)
    {
        auto changes = TreeComporator::compare(prevScan.get(), _lastScanItem.get());
        emit compareFinished(changes);
    }
    else
    {
        emit compareFinished({});
    }
}

void ScanController::doSave()
{
    if (!_lastScanItem) return;
    _database->upsertScanTree(_lastScanItem.get(), _lastPath, _lastTime);
}

void ScanController::doLoad()
{
    auto list = _database->getAllScanIdsAndPaths();
    if (list.isEmpty()) return;

    QStringList paths;
    for (auto& p : list)
    {
        paths << QString("%1: %2").arg(p.first).arg(p.second);
    }

    bool ok = false;
    QString sel = QInputDialog::getItem(nullptr, "Load Scan", "Select scan:", paths, 0, false, &ok);

    if (!ok) return;

    int id = sel.section(":", 0, 0).toInt();
    _lastScanItem = _database->loadScanTree(id);
    buildModel(_lastScanItem.get());
}

void ScanController::doCompare(const QString &path)
{
    if (!_lastScanItem) return;
    auto newTree = _scanner->scan(path);
    auto changes = TreeComporator::compare(_lastScanItem.get(), newTree.get());

}

QStandardItemModel *ScanController::model() const
{
    return _model;
}

void ScanController::addNodeToModel(const ScanItem *item, QStandardItem *parent)
{
    QList<QStandardItem*> row;

    QString fileName = item->name();

    auto* nameItem = new QStandardItem(fileName);
    auto* sizeItem = new QStandardItem(convertFileSizeToHumanReadable(item->size()));

    if (item->type() == ScanItem::Type::Folder)
    {
        fileName += " (" + QString::number(item->childCount()) + ")";
        nameItem->setIcon(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
    }
    else
    {
        nameItem->setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    }

    row << nameItem << sizeItem;
    parent->appendRow(row);

    for (int i = 0; i < item->childCount(); ++i)
    {
        addNodeToModel(item->child(i), row.first());
    }
}

void ScanController::buildModel(const ScanItem *root)
{
    clearModel();
    addNodeToModel(root, _model->invisibleRootItem());
}

QString ScanController::convertFileSizeToHumanReadable(const qint64 & bytes) const
{
    QString number;

    if(bytes < 0x400)
    {
        number = QLocale::system().toString(bytes);
        number.append(" B");
        return number;
    }
    else
    {
        if(bytes >= 0x400 && bytes < 0x100000)
        {
            qint64 result = (bytes + (0x400 / 2)) / 0x400;

            if(result < 0x400)
            {
                number = QLocale::system().toString(result);
                number.append(" KB");
                return number;
            }
            else
            {
                qint64 result = (bytes + (0x100000 / 2)) / 0x100000;
                number = QLocale::system().toString(result);
                number.append(" MB");
                return number;
            }
        }
        else
        {
            if(bytes >= 0x100000 && bytes < 0x40000000)
            {
                qint64 result = (bytes + (0x100000 / 2)) / 0x100000;

                if(result < 0x100000)
                {
                    number = QLocale::system().toString(result);
                    number.append(" MB");
                    return number;
                }
                else
                {
                    qint64 result = (bytes + (0x40000000 / 2)) / 0x40000000;
                    number = QLocale::system().toString(result);
                    number.append(" GB");
                    return number;
                }
            }
            else
            {
                if(bytes >= 0x40000000 && bytes < 0x10000000000)
                {
                    qint64 result = (bytes + (0x40000000 / 2)) / 0x40000000;

                    if(result < 0x40000000)
                    {
                        number = QLocale::system().toString(result);
                        number.append(" GB");
                        return number;
                    }
                    else
                    {
                        qint64 result = (bytes + (0x10000000000 / 2)) / 0x10000000000;
                        number = QLocale::system().toString(result);
                        number.append(" TB");
                        return number;
                    }
                }
                else
                {
                    qint64 result = (bytes + (0x10000000000 / 2)) / 0x10000000000;
                    number = QLocale::system().toString(result);
                    number.append(" TB");
                    return number;
                }
            }
        }
    }
}

void ScanController::clearModel() const
{
    _model->clear();
    _model->setHorizontalHeaderLabels({ "Name", "Size" });
}
