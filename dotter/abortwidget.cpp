#include "abortwidget.h"
#include "ui_abortwidget.h"

AbortWidget::AbortWidget(QWidget* parent) :
    QWidget{parent},
    m_ui{new Ui_AbortWidget}
{
    m_ui->setupUi(this);

    connect(m_ui->button, &QPushButton::clicked, this, &AbortWidget::aborted);
}

AbortWidget::~AbortWidget()
{
    delete m_ui;
}

void AbortWidget::setMessage(const QString& message)
{
    m_ui->label->setText(message);
}
