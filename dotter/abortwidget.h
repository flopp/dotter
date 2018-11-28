#pragma once

#include <QtWidgets/QWidget>

class Ui_AbortWidget;

class AbortWidget: public QWidget
{
    Q_OBJECT
    
    public:
        explicit AbortWidget(QWidget* parent = nullptr);
        virtual ~AbortWidget();
    
        void setMessage(const QString& message);
    
    signals:
        void aborted();
    
    private:
        Ui_AbortWidget* m_ui{nullptr};
};
