#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>
#include <QString>

#include <cassert>

#include "abortwidget.h"
#include "app.h"
#include "config.h"
#include "mainwindow.h"
#include "svgview.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
        QMainWindow(parent), _ui(new Ui_MainWindow), _svgView(new SvgView()), _abortWidget(
                new AbortWidget()), _toolMapper(
                new QSignalMapper(this))
{
    _ui->setupUi(this);
    setWindowTitle("dotter");

    connect(_ui->actionFileOpen, &QAction::triggered, this,
            &MainWindow::showFileOpenDialog);

    connect(_abortWidget, &AbortWidget::aborted, this,
            &MainWindow::stopLayout);

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
    stopLayout();
    delete _ui;
}

void MainWindow::showFileOpenDialog()
{
    stopLayout();

    QString fileName = QFileDialog::getOpenFileName(this, "Open DOT File", "",
            "DOT Files (*.dot);;All Files (*)");

    if (fileName != "")
    {
        openFile(fileName);
    }
}

void MainWindow::openFile(const QString& fileName)
{
    stopLayout();

    if (!QFile::exists(fileName))
    {
        QMessageBox::critical(this, "dotter",
                QString("The file \"%1\" does not exist!").arg(fileName));
        return;
    }

    _fileName = fileName;
    startLayout();
}

void MainWindow::layoutFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    switch (exitStatus)
    {
        case QProcess::NormalExit:
            if (exitCode == 0)
            {
                setWindowTitle(QString("dotter - %1").arg(_fileName));
                _svgView->load(_process->readAllStandardOutput());
            }
            else
            {
                QString msg = QString("%1 returned with exit code %2.\n%3\n%4");
                msg = msg.arg(_lastCommandLine);
                msg = msg.arg(exitCode);
                msg = msg.arg(QString(_process->readAllStandardOutput()));
                msg = msg.arg(QString(_process->readAllStandardError()));
                QMessageBox::critical(this, "dotter", msg);
            }
            break;
        case QProcess::CrashExit:
            QString msg = QString("%1 has crashed.\n%2\n%3");
            msg = msg.arg(_lastCommandLine);
            msg = msg.arg(QString(_process->readAllStandardOutput()));
            msg = msg.arg(QString(_process->readAllStandardError()));
            QMessageBox::critical(this, "dotter", msg);
            break;
    }

    stopLayout();
}

void MainWindow::layoutError(QProcess::ProcessError error)
{
    switch (error)
    {
        case QProcess::FailedToStart:
        {
            QString msg = QString("Failed to start %1.");
            msg = msg.arg(_lastCommandLine);
            QMessageBox::critical(this, "dotter", msg);
            break;
        }
        default:
        {
            QString msg = QString("Unknown error when running %1.");
            msg = msg.arg(_lastCommandLine);
            QMessageBox::critical(this, "dotter", msg);
            break;
        }
    }

    stopLayout();
}

void MainWindow::stopLayout()
{
    _abortWidget->hide();

    if (_process)
    {
        if (_process->state() != QProcess::NotRunning)
        {
            _process->kill();
            _process->waitForFinished();
        }

        delete _process;
        _process = nullptr;
    }
}

void MainWindow::startLayout()
{
    stopLayout();

    assert(!_process);

    if (_fileName.isEmpty())
    {
        return;
    }

    _process = new QProcess(this);
    connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)), this,
            SLOT(layoutFinished(int, QProcess::ExitStatus)));

    connect(_process, &QProcess::errorOccurred, this, &MainWindow::layoutError);

    Config* config = static_cast<App*>(qApp)->config();
    const QString exePath = config->getToolPath(_tool);
    if (exePath.isEmpty())
    {
        QString msg = QString("No configured executable path for tool '%1'.");
        msg = msg.arg(_tool);
        QMessageBox::critical(this, "dotter", msg);
        return;
    }

    _abortWidget->setMessage(
            QString("Computing '%1' layout for '%2'...").arg(_tool,
                    _fileName));
    _abortWidget->show();
    QStringList proc_args;
    proc_args.push_back("-Tsvg");
    proc_args.push_back(_fileName);

    _lastCommandLine = QString("%1 -tsvg %2").arg(exePath).arg(_fileName);

    _process->start(exePath, proc_args);
}

void MainWindow::setLayout(const QString& layout)
{
    if (layout == _tool)
    {
        return;
    }

    stopLayout();
    _tool = layout;
    startLayout();
}
