#pragma once

#include <QObject>
#include <QProcess>

class LayoutProcess : public QObject
{
    Q_OBJECT

    public:
        explicit LayoutProcess(QObject* parent = nullptr);
        virtual ~LayoutProcess();

        LayoutProcess(const LayoutProcess&) = delete;
        LayoutProcess& operator=(const LayoutProcess&) = delete;

    public slots:
        void start(const QString& tool, const QString& fileName);
        void stop();

    signals:
        void started(const QString& commandLine);
        void stopped();
        void finished(const QByteArray& result);
        void failed(const QString& message);

    private slots:
        void layoutFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void layoutError(QProcess::ProcessError error);

    private:
        QProcess* _process{nullptr};
        QString _lastCommandLine;
};
