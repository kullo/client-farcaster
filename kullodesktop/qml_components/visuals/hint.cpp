/* Copyright 2013–2018 Kullo GmbH. All rights reserved. */
#include "hint.h"

namespace KulloDesktop {
namespace QmlComponents {
namespace Visuals {

Hint::Hint(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    direction_ = QStringLiteral("topleft");
    connect(this, &Hint::widthChanged,
            this, &Hint::onAppearanceChanged);
    connect(this, &Hint::heightChanged,
            this, &Hint::onAppearanceChanged);
    connect(this, &Hint::colorChanged,
            this, &Hint::onAppearanceChanged);
}

QColor Hint::color() const
{
    return color_;
}

void Hint::setColor(QColor color)
{
    if (color_ == color) return;

    color_ = color;
    emit colorChanged();
}

QString Hint::direction() const
{
    return direction_;
}

void Hint::setDirection(const QString &direction)
{
    if (direction_ == direction) return;

    direction_ = direction;
    emit directionChanged();
}

quint32 Hint::radius() const
{
    return radius_;
}

void Hint::setRadius(quint32 r)
{
    if (r == radius_) return;

    radius_ = r;
    emit radiusChanged();
}

qreal Hint::paddingTop() const
{
    return paddingTop_;
}

void Hint::setPaddingTop(qreal p)
{
    if (p == paddingTop_) return;

    paddingTop_ = p;
    emit paddingTopChanged();
}

qreal Hint::paddingBottom() const
{
    return paddingBottom_;
}

void Hint::setPaddingBottom(qreal p)
{
    if (p == paddingBottom_) return;

    paddingBottom_ = p;
    emit paddingBottomChanged();
}

void Hint::paint(QPainter *painter)
{
    QPen pen(Qt::NoPen);
    painter->setPen(pen);

    QBrush brush(color_);
    painter->setBrush(brush);
    painter->setRenderHints(QPainter::Antialiasing, true);

    qreal w = width();
    qreal h = height();

    QRectF rectangle;
    // top-left
    rectangle = QRectF(0, paddingTop_, 2*radius_, 2*radius_);
    painter->drawPie(rectangle, START_W * 16, 90 * -1 * 16);

    // top-right
    rectangle = QRectF(w-2*radius_, paddingTop_, 2*radius_, 2*radius_);
    painter->drawPie(rectangle, START_N * 16, 90 * -1 * 16);

    // bottom-right
    rectangle = QRectF(w-2*radius_, h-paddingBottom_-2*radius_, 2*radius_, 2*radius_);
    painter->drawPie(rectangle, START_E * 16, 90 * -1 * 16);

    // bottom-left
    rectangle = QRectF(0, h-paddingBottom_-2*radius_, 2*radius_, 2*radius_);
    painter->drawPie(rectangle, START_S * 16, 90 * -1 * 16);

    QPointF points[12] = {
        QPointF(radius_, paddingTop_),
        QPointF(radius_, paddingTop_+radius_),
        QPointF(0, paddingTop_+radius_),
        QPointF(0, h-paddingBottom_-radius_),
        QPointF(radius_, h-paddingBottom_-radius_),
        QPointF(radius_, h-paddingBottom_),
        QPointF(w-radius_, h-paddingBottom_),
        QPointF(w-radius_, h-paddingBottom_-radius_),
        QPointF(w, h-paddingBottom_-radius_),
        QPointF(w, paddingTop_+radius_),
        QPointF(w-radius_, paddingTop_+radius_),
        QPointF(w-radius_, paddingTop_)
    };
    painter->drawPolygon(points, 12);

    if (direction_ == "topright")
    {
        QPointF pointerPoints[3] = {
            QPointF(0.75*w + 2*radius_, 0),
            QPointF(0.75*w + 2*radius_, paddingTop_),
            QPointF(0.75*w, paddingTop_)
        };
        painter->drawPolygon(pointerPoints, 3);
    }
    else if (direction_ == "topleft")
    {
        QPointF pointerPoints[3] = {
            QPointF(0.25*w, 0),
            QPointF(0.25*w + 2*radius_, paddingTop_),
            QPointF(0.25*w, paddingTop_)
        };
        painter->drawPolygon(pointerPoints, 3);
    }
    else if (direction_ == "bottomleft")
    {
        QPointF pointerPoints[3] = {
            QPointF(0.25*w, h),
            QPointF(0.25*w + 2*radius_, h-paddingBottom_),
            QPointF(0.25*w, h-paddingBottom_)
        };
        painter->drawPolygon(pointerPoints, 3);
    }
    else if (direction_ == "bottomright")
    {
        QPointF pointerPoints[3] = {
            QPointF(0.75*w + 2*radius_, h),
            QPointF(0.75*w + 2*radius_, h-paddingBottom_),
            QPointF(0.75*w, h-paddingBottom_)
        };
        painter->drawPolygon(pointerPoints, 3);
    }
}

void Hint::onAppearanceChanged()
{
    int new_width = width();
    int new_height = height();
    update(QRect(0, 0, new_width, new_height));
}

}
}
}
