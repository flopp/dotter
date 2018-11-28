#include <QtCore/QDebug>
#include <QtCore/QSignalMapper>
#include <QtCore/QString>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QToolButton>

#include <cassert>

#include "app.h"
#include "config.h"
#include "layoutprocess.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
        QMainWindow{parent},
        _ui{new Ui_MainWindow},
        _process{new LayoutProcess{this}},
        _toolMapper{new QSignalMapper{this}}
{
    _ui->setupUi(this);
    setWindowTitle(qApp->applicationName());

    // setup toolbar
    _openAction = _ui->toolBar->addAction(loadIcon("document-open"), "Open...", this, &MainWindow::showFileOpenDialog);
    QMenu* layoutMenu{new QMenu{}};
    auto layoutGroup{new QActionGroup{this}};
    _layoutDotAction   = addLayoutAction(layoutGroup, layoutMenu, "Dot", "dot");
    _layoutTwopiAction = addLayoutAction(layoutGroup, layoutMenu, "Twopi", "twopi");
    _layoutCircoAction = addLayoutAction(layoutGroup, layoutMenu, "Circo", "circo");
    _layoutNeatoAction = addLayoutAction(layoutGroup, layoutMenu, "Neato", "neato");
    _layoutFdpAction   = addLayoutAction(layoutGroup, layoutMenu, "FDP", "fdp");
    QToolButton* layoutButton{new QToolButton{}};
    layoutButton->setMenu(layoutMenu);
    layoutButton->setPopupMode(QToolButton::InstantPopup);
    layoutButton->setIcon(loadIcon("document-page-setup"));
    _ui->toolBar->addWidget(layoutButton);
    _zoomFitAction = _ui->toolBar->addAction(loadIcon("zoom-fit-best"), "Zoom Fit", _ui->view, &SvgView::zoomFit);

    connect(_process, &LayoutProcess::started, this, &MainWindow::layoutStarted);
    connect(_process, &LayoutProcess::stopped, this, &MainWindow::layoutStopped);
    connect(_process, &LayoutProcess::finished, this, &MainWindow::layoutFinished);
    connect(_process, &LayoutProcess::failed, this, &MainWindow::layoutError);

    connect(_ui->abort, &AbortWidget::aborted, _process, &LayoutProcess::stop);
    _ui->abort->hide();

    connect(_toolMapper, QOverload<const QString&>::of(&QSignalMapper::mapped), this, &MainWindow::setLayout);

    _layoutDotAction->setChecked(true);
}

MainWindow::~MainWindow()
{
    _process->stop();
    delete _ui;
}

void MainWindow::showFileOpenDialog()
{
    _process->stop();

    QString fileName{QFileDialog::getOpenFileName(this, "Open DOT File", "", "DOT Files (*.dot);;All Files (*)")};

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
        QMessageBox::critical(this, qApp->applicationName(), QString("The file \"%1\" does not exist!").arg(fileName));
        return;
    }

    _fileName = fileName;
    _process->start(_tool, fileName);
}

void MainWindow::layoutStarted(const QString& commandLine)
{
    _ui->abort->setMessage(QString("Running '%1'...").arg(commandLine));
    _ui->abort->show();
}

void MainWindow::layoutStopped()
{
    _ui->abort->hide();
}

void MainWindow::layoutFinished(const QByteArray& result)
{
    setWindowTitle(QString("%1 - %2").arg(qApp->applicationName()).arg(_fileName));
    _ui->view->load(result);
}

void MainWindow::layoutError(const QString& message)
{
    QMessageBox::critical(this, qApp->applicationName(), message);
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

QIcon MainWindow::loadIcon(QString name) const {
    if (QIcon::hasThemeIcon(name)) {
        return QIcon::fromTheme(name);
    }
    return QIcon{};
}

QAction* MainWindow::addLayoutAction(QActionGroup* group, QMenu* menu, QString label, QString command)
{
    QAction* action{new QAction{label}};
    action->setCheckable(true);
    menu->addAction(action);
    group->addAction(action);
    connect(action, &QAction::triggered, _toolMapper, QOverload<>::of(&QSignalMapper::map));
    _toolMapper->setMapping(action, command);
    return action;
}
