#pragma once

#include <QtWidgets/QMainWindow>

class LayoutProcess;
class QActionGroup;
class QMenu;
class QSignalMapper;
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
        QIcon loadIcon(QString name) const;
        QAction* addLayoutAction(QActionGroup* group, QMenu* menu, QString label, QString command);

    private:
        Ui_MainWindow* _ui{nullptr};
        LayoutProcess* _process{nullptr};
        QString _fileName;
        QString _tool{"dot"};
        QSignalMapper* _toolMapper{nullptr};

        QAction* _openAction{nullptr};
        QAction* _zoomFitAction{nullptr};
        QAction* _layoutDotAction{nullptr};
        QAction* _layoutTwopiAction{nullptr};
        QAction* _layoutCircoAction{nullptr};
        QAction* _layoutNeatoAction{nullptr};
        QAction* _layoutFdpAction{nullptr};
};
