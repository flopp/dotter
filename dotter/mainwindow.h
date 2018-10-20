#pragma once

#include <QMainWindow>
#include <QProcess>

class QSignalMapper;

class AbortWidget;
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
        void layoutFinished(int exitCode, QProcess::ExitStatus exitStatus);
        void stopLayout();
        void startLayout();
        void setLayout(const QString& layout);

    private:
        Ui_MainWindow* _ui{ nullptr };
        SvgView* _svgView{ nullptr };
        AbortWidget* _abortWidget{ nullptr };
        QProcess* _process{ nullptr };
        QString _fileName;
        QString _tool{ "dot" };
        QSignalMapper* _toolMapper{ nullptr };
};
