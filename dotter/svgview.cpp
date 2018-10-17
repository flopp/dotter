#include "svgview.h"

#include <QSvgRenderer>

#include <QApplication>
#include <QPainter>
#include <QWheelEvent>
#include <QtDebug>

SvgView::SvgView(QWidget* parent) :
    QWidget{parent},
    m_renderer{new QSvgRenderer{this}}
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    
    connect(m_renderer, SIGNAL(repaintNeeded()), this, SLOT(update()));
}

void SvgView::load(const QString& fileName)
{
    if (!(m_renderer->load(fileName)))
    {
        qDebug() << "Loading SVG from File failed" << endl;
    }

    zoomFit();
}

void SvgView::load(const QByteArray& svgData)
{
    if (!(m_renderer->load(svgData)))
    {
        qDebug() << "Loading SVG from QByteArray failed" << endl;
    }

    zoomFit();
}

void SvgView::zoomIn()
{
    m_scale *= 1.1;
    update();
}

void SvgView::zoomOut()
{
    m_scale *= 0.9;
    update();
}

void SvgView::zoomFit()
{
    if (m_renderer->isValid())
    {
        const QRectF& viewBox{m_renderer->viewBox()};
                
        if ((width() * viewBox.height()) > (height() * viewBox.width()))
        {
            m_scale = height() / viewBox.height();
        }
        else
        {
            m_scale = width() / viewBox.width();
        }
        
        m_translation = QPointF{0, 0};
        
        update();
    }
}

void SvgView::paintEvent(QPaintEvent*)
{
    QPainter p{this};
    p.fillRect(rect(), Qt::white);

    if (!(m_renderer->isValid())) {
        return;
    }
        
    p.setWindow(rect());
    
    p.scale(m_scale, m_scale);
    p.translate(m_translation);
    m_renderer->render(&p, m_renderer->viewBox());
}

void SvgView::wheelEvent(QWheelEvent* event)
{
    double oldScale{m_scale};
    
    if (event->delta() > 0)
    {
        m_scale *= 1.1;
    }
    else
    {
        m_scale *= 0.9;
    }
    
    m_translation += ((1.0 / m_scale) - (1.0 / oldScale)) * event->pos();
    
    
    update();
}

void SvgView::mousePressEvent(QMouseEvent* event)
{
    m_mousePressPos = event->pos();
    m_startTranslation = m_translation;
    
    event->accept();
}

void SvgView::mouseMoveEvent(QMouseEvent* event)
{
    if (m_mousePressPos.isNull())
    {
        event->ignore();
        return;
    }

    m_translation = m_startTranslation + (QPointF{event->pos() - m_mousePressPos} / m_scale);

    event->accept();

    update();
}

void SvgView::mouseReleaseEvent(QMouseEvent* event)
{
    m_mousePressPos = QPoint{};
    event->accept();
}
