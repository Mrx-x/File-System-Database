#include "Controller/ScanController.h"
#include "Model/ScanItem.h"
#include "Database/IDatabaseService.h"

#include <QStandardItem>
#include <QList>
#include <QLocale>
#include <QDateTime>

ScanController::ScanController(IScanner *scanner, const DatabasePtr& database, QObject *parent)
    : QObject(parent)
    , _scanner(scanner)
    , _database(database)
{
    _model = new QStandardItemModel(this);
    _model->setHorizontalHeaderLabels({ "Name", "Size" });
}

void ScanController::startScan(const QString &path)
{
    auto root = _scanner->scan(path);
    _database->saveScanTree(root.get(), path, QDateTime::currentDateTime());
    _model->clear();
    _model->setHorizontalHeaderLabels({ "Name", "Size" });
    buildModel(root.get());
}

void ScanController::doLoad(int scanId)
{
    auto loaded = _database->loadScanTree(scanId);
    buildModel(loaded.get());
}

QStandardItemModel *ScanController::model() const
{
    return _model;
}

void ScanController::addNodeToModel(const ScanItem *item, QStandardItem *parent)
{
    QList<QStandardItem*> row;

    QString fileName = item->name();

    if (item->type() == ScanItem::Type::Folder)
    {
        fileName += " (" + QString::number(item->childCount()) + ")";
    }

    row << new QStandardItem(fileName)
        << new QStandardItem(convertFileSizeToHumanReadable(item->size()));

    parent->appendRow(row);

    for (int i = 0; i < item->childCount(); ++i)
    {
        addNodeToModel(item->child(i), row.first());
    }
}

void ScanController::buildModel(const ScanItem *root)
{
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
