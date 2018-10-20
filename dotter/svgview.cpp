#include "svgview.h"

#include <QSvgRenderer>

#include <QApplication>
#include <QPainter>
#include <QWheelEvent>
#include <QtDebug>

SvgView::SvgView(QWidget* parent) :
    QWidget{parent},
    _renderer{new QSvgRenderer{this}}
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    
    connect(_renderer, SIGNAL(repaintNeeded()), this, SLOT(update()));
}

SvgView::~SvgView()
{}

void SvgView::load(const QString& fileName)
{
    if (!(_renderer->load(fileName)))
    {
        qDebug() << "Loading SVG from File failed" << endl;
    }

    zoomFit();
}

void SvgView::load(const QByteArray& svgData)
{
    if (!(_renderer->load(svgData)))
    {
        qDebug() << "Loading SVG from QByteArray failed" << endl;
    }

    zoomFit();
}

void SvgView::zoomIn()
{
    _scale *= 1.1;
    update();
}

void SvgView::zoomOut()
{
    _scale *= 0.9;
    update();
}

void SvgView::zoomFit()
{
    if (_renderer->isValid())
    {
        const QRectF& viewBox{_renderer->viewBox()};
                
        if ((width() * viewBox.height()) > (height() * viewBox.width()))
        {
            _scale = height() / viewBox.height();
        }
        else
        {
            _scale = width() / viewBox.width();
        }
        
        _translation = QPointF{0, 0};
        
        update();
    }
}

void SvgView::paintEvent(QPaintEvent*)
{
    QPainter p{this};
    p.fillRect(rect(), Qt::white);

    if (!(_renderer->isValid())) {
        return;
    }
        
    p.setWindow(rect());
    
    p.scale(_scale, _scale);
    p.translate(_translation);
    _renderer->render(&p, _renderer->viewBox());
}

void SvgView::wheelEvent(QWheelEvent* event)
{
    double oldScale{_scale};
    
    if (event->delta() > 0)
    {
        _scale *= 1.1;
    }
    else
    {
        _scale *= 0.9;
    }
    
    _translation += ((1.0 / _scale) - (1.0 / oldScale)) * event->pos();
    
    update();
}

void SvgView::mousePressEvent(QMouseEvent* event)
{
    _mousePressPos = event->pos();
    _startTranslation = _translation;
    
    event->accept();
}

void SvgView::mouseMoveEvent(QMouseEvent* event)
{
    if (_mousePressPos.isNull())
    {
        event->ignore();
        return;
    }

    _translation = _startTranslation + (QPointF{event->pos() - _mousePressPos} / _scale);

    event->accept();

    update();
}

void SvgView::mouseReleaseEvent(QMouseEvent* event)
{
    _mousePressPos = QPoint{};
    event->accept();
}
