/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#include "advancedrectangle.h"

namespace KulloDesktop {
namespace Visuals {

AdvancedRectangle::AdvancedRectangle(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    connect(this, &AdvancedRectangle::widthChanged,
            this, &AdvancedRectangle::onAppearanceChanged);
    connect(this, &AdvancedRectangle::heightChanged,
            this, &AdvancedRectangle::onAppearanceChanged);
    connect(this, &AdvancedRectangle::colorChanged,
            this, &AdvancedRectangle::onAppearanceChanged);
}

QColor AdvancedRectangle::color() const
{
    return color_;
}

void AdvancedRectangle::setColor(QColor color)
{
    if (color_ == color) return;

    color_ = color;
    emit colorChanged();
}

qreal AdvancedRectangle::radius() const
{
    return radius_;
}

void AdvancedRectangle::setRadius(qreal r)
{
    if (r == radius_) return;

    radius_ = r;
    emit radiusChanged();
}

qreal AdvancedRectangle::radiusTopLeft() const
{
    if (radiusTopLeft_ >= 0) return radiusTopLeft_;
    else return radius_;
}

void AdvancedRectangle::setRadiusTopLeft(qreal r)
{
    if (r == radiusTopLeft_) return;

    radiusTopLeft_ = r;
    emit radiusTopLeftChanged();
}

qreal AdvancedRectangle::radiusTopRight() const
{
    if (radiusTopRight_ >= 0) return radiusTopRight_;
    else return radius_;
}

void AdvancedRectangle::setRadiusTopRight(qreal r)
{
    if (r == radiusTopRight_) return;

    radiusTopRight_ = r;
    emit radiusTopRightChanged();
}

qreal AdvancedRectangle::radiusBottomRight() const
{
    if (radiusBottomRight_ >= 0) return radiusBottomRight_;
    else return radius_;
}

void AdvancedRectangle::setRadiusBottomRight(qreal r)
{
    if (r == radiusBottomRight_) return;

    radiusBottomRight_ = r;
    emit radiusBottomRightChanged();
}

qreal AdvancedRectangle::radiusBottomLeft() const
{
    if (radiusBottomLeft_ >= 0) return radiusBottomLeft_;
    else return radius_;
}

void AdvancedRectangle::setRadiusBottomLeft(qreal r)
{
    if (r == radiusBottomLeft_) return;

    radiusBottomLeft_ = r;
    emit radiusBottomLeftChanged();
}

void AdvancedRectangle::paint(QPainter *painter)
{
    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    QBrush brush(color_);
    painter->setBrush(brush);

    painter->setRenderHints(QPainter::Antialiasing, true);

    int _width = width();
    int _height = height();

    QRectF rectangle;
    // top-left
    rectangle = QRectF(0, 0, 2*radiusTopLeft(), 2*radiusTopLeft());
    painter->drawPie(rectangle, START_W * 16, 90 * -1 * 16);

    // top-right
    rectangle = QRectF(_width-2*radiusTopRight(), 0, 2*radiusTopRight(), 2*radiusTopRight());
    painter->drawPie(rectangle, START_N * 16, 90 * -1 * 16);

    // bottom-right
    rectangle = QRectF(_width-2*radiusBottomRight(), _height-2*radiusBottomRight(), 2*radiusBottomRight(), 2*radiusBottomRight());
    painter->drawPie(rectangle, START_E * 16, 90 * -1 * 16);

    // bottom-left
    rectangle = QRectF(0, _height-2*radiusBottomLeft(), 2*radiusBottomLeft(), 2*radiusBottomLeft());
    painter->drawPie(rectangle, START_S * 16, 90 * -1 * 16);

    QPointF points[12] = {
        QPointF(radiusTopLeft(), 0),
        QPointF(radiusTopLeft(), radiusTopLeft()),
        QPointF(0, radiusTopLeft()),
        QPointF(0, _height-radiusBottomLeft()),
        QPointF(radiusBottomLeft(), _height-radiusBottomLeft()),
        QPointF(radiusBottomLeft(), _height),
        QPointF(_width-radiusBottomRight(), _height),
        QPointF(_width-radiusBottomRight(), _height-radiusBottomRight()),
        QPointF(_width, _height-radiusBottomRight()),
        QPointF(_width, radiusTopRight()),
        QPointF(_width-radiusTopRight(), radiusTopRight()),
        QPointF(_width-radiusTopRight(), 0)
    };
    painter->drawPolygon(points, 12);
}

void AdvancedRectangle::onAppearanceChanged()
{
    int new_width = width();
    int new_height = height();
    update(QRect(0, 0, new_width, new_height));
}

}
}
