#include "Controller/ScanController.h"
#include "Database/IDatabaseService.h"

#include <QStandardItem>
#include <QList>
#include <QLocale>
#include <QDateTime>
#include <QInputDialog>

ScanController::ScanController(IScanner *scanner, const DatabasePtr& database, QObject *parent)
    : QObject(parent)
    , _scanner(scanner)
    , _database(database)
{
    _model = new QStandardItemModel(this);
    _model->setHorizontalHeaderLabels({ "Name", "Size" });
}

void ScanController::doScan(const QString &path)
{
    _lastScanItem = _scanner->scan(path);
    _lastPath = path;
    _lastTime = QDateTime::currentDateTime();
    buildModel(_lastScanItem.get());
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
