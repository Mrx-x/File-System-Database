#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "Scanner/DirectoryScanner.h"
#include "Controller/ScanController.h"
#include "Database/SQLiteDatabaseService.h"

#include <QFileDialog>
#include <QInputDialog>

const char* const DatabaseName = "scans.db";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _database(nullptr)
{
    ui->setupUi(this);

    auto scanner = new DirectoryScanner(this);
    _database = std::make_shared<SQLiteDatabaseService>();
    _database->open(DatabaseName);

    _controller = new ScanController(scanner, _database, this);

    ui->treeView->setModel(_controller->model());

    connect(ui->btnScan, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select folder");
        if (!dir.isEmpty())
        {
            _controller->doScan(dir);
        }
    });
    connect(ui->btnLoad, &QPushButton::clicked, _controller, &ScanController::doLoad);
    connect(ui->btnSave, &QPushButton::clicked, _controller, &ScanController::doSave);
    connect(_controller, &ScanController::compareFinished, this, &MainWindow::onCompareFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onCompareFinished(const std::vector<ChangeEntry> &changes)
{
    populateChangeList(changes);
}

void MainWindow::populateChangeList(const std::vector<ChangeEntry> &changes)
{
    ui->changeList->clear();

    if (changes.empty())
    {
        ui->changeList->addItem("No changes detected");
        return;
    }

    for (auto& e : changes)
    {
        QString prefix;
        switch (e.type)
        {
        case ScanItem::Change::New: prefix = "A: "; break;
        case ScanItem::Change::Deleted: prefix = "D: "; break;
        case ScanItem::Change::Modified: prefix = "M: "; break;
        default: prefix = " "; break;
        }

        auto item = new QListWidgetItem(prefix + e.path, ui->changeList);

        ui->changeList->addItem(item);
    }
}
