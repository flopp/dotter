#include "app.h"
#include "config.h"
#include "layoutprocess.h"
#include <cassert>
#include <QDebug>

LayoutProcess::LayoutProcess(QObject* parent) :
    QObject(parent)
{
}

LayoutProcess::~LayoutProcess()
{
    stop();
}

void LayoutProcess::start(const QString& tool, const QString& fileName)
{
    stop();

    assert(!_process);

    if (fileName.isEmpty())
    {
        return;
    }

    Config* config = static_cast<App*>(qApp)->config();
    const QString exePath = config->getToolPath(tool);
    if (exePath.isEmpty())
    {
        QString msg = QString("No configured executable path for tool '%1'.");
        msg = msg.arg(tool);
        emit failed(msg);
        return;
    }

    _process = new QProcess(this);
    connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(layoutFinished(int, QProcess::ExitStatus)));

    connect(_process, &QProcess::errorOccurred, this, &LayoutProcess::layoutError);

    _lastCommandLine = QString("%1 -Tsvg %2").arg(exePath).arg(fileName);
    emit started(_lastCommandLine);

    QStringList proc_args;
    proc_args.push_back("-Tsvg");
    proc_args.push_back(fileName);
    _process->start(exePath, proc_args);
}

void LayoutProcess::stop()
{
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

    emit stopped();
}

void LayoutProcess::layoutFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    assert(_process);

    const QByteArray out = _process->readAllStandardOutput();
    const QByteArray err = _process->readAllStandardError();

    stop();

    switch (exitStatus)
    {
        case QProcess::NormalExit:
            if (exitCode == 0)
            {
                emit finished(out);
            }
            else
            {
                QString msg = QString("'%1' returned with exit code %2.\n\nMessage:\n%3");
                msg = msg.arg(_lastCommandLine);
                msg = msg.arg(exitCode);
                auto err_s = QString(err);
                if (err_s.length() > 200)
                {
                    err_s = err_s.left(200) + " [...]";
                }
                msg = msg.arg(err_s);
                emit failed(msg);
            }
            break;
        case QProcess::CrashExit:
            QString msg = QString("'%1' has crashed.\n\nMessage:\n%2");
            msg = msg.arg(_lastCommandLine);
            auto err_s = QString(err);
            if (err_s.length() > 200)
            {
                err_s = err_s.left(200) + " [...]";
            }
            msg = msg.arg(err_s);
            emit failed(msg);
            break;
    }
}

void LayoutProcess::layoutError(QProcess::ProcessError error)
{
    stop();

    switch (error)
    {
        case QProcess::FailedToStart:
        {
            QString msg = QString("Failed to run '%1'.");
            msg = msg.arg(_lastCommandLine);
            emit failed(msg);
            break;
        }
        default:
        {
            QString msg = QString("Unknown error when running '%1'.");
            msg = msg.arg(_lastCommandLine);
            emit failed(msg);
            break;
        }
    }
}
