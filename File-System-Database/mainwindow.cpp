#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "Scanner/DirectoryScanner.h"
#include "Controller/ScanController.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    auto scanner = new DirectoryScanner(this);
    _controller = new ScanController(scanner, this);

    ui->treeView->setModel(_controller->model());

    connect(ui->btnScan, &QPushButton::clicked, this, [this]() {
        QString dir = QFileDialog::getExistingDirectory(this, "Select folder");
        if (!dir.isEmpty())
        {
            _controller->startScan(dir);
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
