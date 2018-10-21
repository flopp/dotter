#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <QString>

#include <cassert>

#include "abortwidget.h"
#include "app.h"
#include "config.h"
#include "layoutprocess.h"
#include "mainwindow.h"
#include "svgview.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent),
        _ui(new Ui_MainWindow),
        _svgView(new SvgView()),
        _abortWidget(new AbortWidget()),
        _process(new LayoutProcess(this)),
        _toolMapper(new QSignalMapper(this))
{
    _ui->setupUi(this);
    setWindowTitle("dotter");

    connect(_ui->actionFileOpen, &QAction::triggered, this,
            &MainWindow::showFileOpenDialog);

    connect(_process, &LayoutProcess::started, this, &MainWindow::layoutStarted);
    connect(_process, &LayoutProcess::stopped, this, &MainWindow::layoutStopped);
    connect(_process, &LayoutProcess::finished, this, &MainWindow::layoutFinished);
    connect(_process, &LayoutProcess::failed, this, &MainWindow::layoutError);

    connect(_abortWidget, &AbortWidget::aborted, _process,
            &LayoutProcess::stop);

    QWidget* container = new QWidget(this);
    {
        auto layout = new QVBoxLayout(container);
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(_svgView);
        layout->addWidget(_abortWidget);
    }
    setCentralWidget(container);
    _abortWidget->hide();

    connect(_toolMapper, SIGNAL(mapped(const QString&)), this,
            SLOT(setLayout(const QString&)));

    _ui->actionLayoutDot->setProperty(  "layout", "dot");
    _ui->actionLayoutTwopi->setProperty("layout", "twopi");
    _ui->actionLayoutCirco->setProperty("layout", "circo");
    _ui->actionLayoutNeato->setProperty("layout", "neato");
    _ui->actionLayoutFdp->setProperty(  "layout", "fdp");

    auto group = new QActionGroup(this);
    for (auto action :
    { _ui->actionLayoutDot, _ui->actionLayoutTwopi, _ui->actionLayoutCirco,
            _ui->actionLayoutNeato, _ui->actionLayoutFdp })
    {
        action->setCheckable(true);
        group->addAction(action);
        connect(action, SIGNAL(triggered()), _toolMapper, SLOT(map()));
        _toolMapper->setMapping(action,
                action->property("layout").toString());
    }

    _ui->actionLayoutDot->setChecked(true);
}

MainWindow::~MainWindow()
{
    _process->stop();
    delete _ui;
}

void MainWindow::showFileOpenDialog()
{
    _process->stop();

    QString fileName = QFileDialog::getOpenFileName(this, "Open DOT File", "",
            "DOT Files (*.dot);;All Files (*)");

    if (fileName != "")
    {
        openFile(fileName);
    }
}

void MainWindow::openFile(const QString& fileName)
{
    _process->stop();

    if (!QFile::exists(fileName))
    {
        QMessageBox::critical(this, "dotter",
                QString("The file \"%1\" does not exist!").arg(fileName));
        return;
    }

    _fileName = fileName;
    _process->start(_tool, fileName);
}

void MainWindow::layoutStarted(const QString& commandLine)
{
    _abortWidget->setMessage(QString("Running '%1'...").arg(commandLine));
    _abortWidget->show();
}

void MainWindow::layoutStopped()
{
    _abortWidget->hide();
}

void MainWindow::layoutFinished(const QByteArray& result)
{
    setWindowTitle(QString("dotter - %1").arg(_fileName));
    _svgView->load(result);
}

void MainWindow::layoutError(const QString& message)
{
    QMessageBox::critical(this, "dotter", message);
}

void MainWindow::setLayout(const QString& layout)
{
    if (layout == _tool)
    {
        return;
    }

    _process->stop();
    _tool = layout;
    _process->start(_tool, _fileName);
}
