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
}

MainWindow::~MainWindow()
{
    delete ui;
}
