#pragma once

#include <QMainWindow>

class QSignalMapper;

class AbortWidget;
class LayoutProcess;
class SvgView;
class Ui_MainWindow;

class MainWindow: public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        virtual ~MainWindow();

    public slots:
        void showFileOpenDialog();
        void openFile(const QString& fileName);

    private slots:
        void layoutStarted(const QString& commandLine);
        void layoutStopped();
        void layoutFinished(const QByteArray& result);
        void layoutError(const QString& message);
        void setLayout(const QString& layout);

    private:
        Ui_MainWindow* _ui{ nullptr };
        SvgView* _svgView{ nullptr };
        AbortWidget* _abortWidget{ nullptr };
        LayoutProcess* _process{nullptr};
        QString _fileName;
        QString _tool{ "dot" };
        QSignalMapper* _toolMapper{ nullptr };
};
