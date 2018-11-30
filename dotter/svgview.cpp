#include "svgview.h"

#include <QtCore/QtDebug>
#include <QtGui/QPainter>
#include <QtGui/QWheelEvent>
#include <QtSvg/QSvgRenderer>
#include <QtWidgets/QApplication>

SvgView::SvgView(QWidget* parent) :
    QWidget{parent},
    _renderer{new QSvgRenderer{this}}
{
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    
    connect(_renderer, &QSvgRenderer::repaintNeeded, this, QOverload<>::of(&SvgView::update));
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
        
        _translation = 0.5 * QPointF{(width() / _scale) - viewBox.width(), (height() / _scale) - viewBox.height()};
        
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
