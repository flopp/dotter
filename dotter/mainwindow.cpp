#include <QFileDialog>
#include <QMessageBox>
#include <QSignalMapper>

#include "abortwidget.h"
#include "mainwindow.h"
#include "svgview.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow{parent},
    m_ui{new Ui_MainWindow},
    m_svgView{new SvgView{}},
    m_abortWidget{new AbortWidget{}},
    m_layoutProcess{new QProcess{this}},
    m_layoutMapper{new QSignalMapper{this}}
{
    m_ui->setupUi(this);

    connect(m_ui->actionFileOpen, &QAction::triggered, this, &MainWindow::showFileOpenDialog);

    connect(m_abortWidget, &AbortWidget::aborted, this, &MainWindow::stopLayout);
    
    QWidget* container{new QWidget{this}};
    {
        auto layout{new QVBoxLayout{container}};
        layout->setSpacing(0);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_svgView);
        layout->addWidget(m_abortWidget);
    }
    setCentralWidget(container);
    m_abortWidget->hide();

    connect(m_layoutProcess, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(layoutFinished(int, QProcess::ExitStatus)));

    connect(m_layoutMapper, SIGNAL(mapped(const QString&)), this, SLOT(setLayout(const QString&)));

    m_ui->actionLayoutDot->setProperty("layout", "dot");
    m_ui->actionLayoutTwopi->setProperty("layout", "twopi");
    m_ui->actionLayoutCirco->setProperty("layout", "circo");
    m_ui->actionLayoutNeato->setProperty("layout", "neato");
    m_ui->actionLayoutFdp->setProperty("layout", "fdp");
    
    auto group{new QActionGroup{this}};
    for (auto action: {m_ui->actionLayoutDot, m_ui->actionLayoutTwopi, m_ui->actionLayoutCirco, m_ui->actionLayoutNeato, m_ui->actionLayoutFdp})
    {
        action->setCheckable(true);
        group->addAction(action);
        connect(action, SIGNAL(triggered()), m_layoutMapper, SLOT(map()));
        m_layoutMapper->setMapping(action, action->property("layout").toString());
    }
    
    m_ui->actionLayoutDot->setChecked(true);
}

MainWindow::~MainWindow()
{
    delete m_ui;
}

void MainWindow::showFileOpenDialog()
{
    stopLayout();
    
    QString fileName = QFileDialog::getOpenFileName(this, "Open DOT File", "", "*.dot" );
    
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
        QMessageBox::critical(this, "dotter", QString("The file \"%1\" does not exist!").arg(fileName));
        return;
    }
    
    m_fileName = fileName;
    startLayout();
}

void MainWindow::layoutFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_abortWidget->hide();
    if ((exitStatus == QProcess::NormalExit) && (exitCode == 0))
    {
        m_svgView->load(m_layoutProcess->readAll());
    }
}

void MainWindow::stopLayout()
{
    m_abortWidget->hide();
    if (m_layoutProcess->state() != QProcess::NotRunning)
    {
        m_layoutProcess->kill();
        m_layoutProcess->waitForFinished();
    }
}

void MainWindow::startLayout()
{
    stopLayout();
    
    if (!m_fileName.isEmpty())
    {
        m_abortWidget->setMessage(QString("Computing '%1' layout for '%2'...").arg(m_layout, m_fileName));
        m_abortWidget->show();
        QStringList proc_args;
        proc_args.push_back("-Tsvg");
        proc_args.push_back(m_fileName);
        
        m_layoutProcess->start(m_layout, proc_args);
    }    
}

void MainWindow::setLayout(const QString& layout)
{
    if (layout == m_layout)
    {
        return;
    }
    
    stopLayout();
    m_layout = layout;
    startLayout();
}