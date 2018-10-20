#pragma once

#include <QtSvg>
#include <QWidget>

class QPaintEvent;
class QSvgRenderer;
class QWheelEvent;
class QMouseEvent;

class SvgView: public QWidget
{
    Q_OBJECT

    public:
        explicit SvgView(QWidget* parent = nullptr);
        virtual ~SvgView();

    public slots:
        void load(const QString& filename);
        void load(const QByteArray& svgdata);

        void zoomIn();
        void zoomOut();
        void zoomFit();

    protected:
        virtual void paintEvent(QPaintEvent* event);
        virtual void wheelEvent(QWheelEvent* event);

        virtual void mousePressEvent(QMouseEvent* event);
        virtual void mouseMoveEvent(QMouseEvent* event);
        virtual void mouseReleaseEvent(QMouseEvent* event);

    private:
        double _scale{1.0};
        QPointF _translation{QPointF{0, 0}};
        QPointF _startTranslation;
        QPoint _mousePressPos;
        QSvgRenderer* _renderer{nullptr};
};
