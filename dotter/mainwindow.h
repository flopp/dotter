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
        void layoutFinished( int exitCode, QProcess::ExitStatus exitStatus);
        void stopLayout();
        void startLayout();
        void setLayout(const QString& layout);

    private:
        Ui_MainWindow* m_ui{nullptr};    
        SvgView* m_svgView{nullptr};
        AbortWidget* m_abortWidget{nullptr};
        QProcess* m_layoutProcess{nullptr};
        QString m_fileName;
        QString m_layout{"dot"};
        QSignalMapper* m_layoutMapper{nullptr};
};
