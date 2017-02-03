/* Copyright 2013–2016 Kullo GmbH. All rights reserved. */
#pragma once

#include <QQuickPaintedItem>
#include <QPainter>
#include <QPen>

namespace KulloDesktop {
namespace QmlComponents {
namespace Visuals {

class AdvancedRectangle : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit AdvancedRectangle(QQuickItem *parent = 0);

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    QColor color() const;
    void setColor(QColor color);

    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
    qreal radius() const;
    void setRadius(qreal r);

    Q_PROPERTY(qreal radiusTopLeft READ radiusTopLeft WRITE setRadiusTopLeft NOTIFY radiusTopLeftChanged)
    qreal radiusTopLeft() const;
    void setRadiusTopLeft(qreal r);

    Q_PROPERTY(qreal radiusTopRight READ radiusTopRight WRITE setRadiusTopRight NOTIFY radiusTopRightChanged)
    qreal radiusTopRight() const;
    void setRadiusTopRight(qreal r);

    Q_PROPERTY(qreal radiusBottomRight READ radiusBottomRight WRITE setRadiusBottomRight NOTIFY radiusBottomRightChanged)
    qreal radiusBottomRight() const;
    void setRadiusBottomRight(qreal r);

    Q_PROPERTY(qreal radiusBottomLeft READ radiusBottomLeft WRITE setRadiusBottomLeft NOTIFY radiusBottomLeftChanged)
    qreal radiusBottomLeft() const;
    void setRadiusBottomLeft(qreal r);

    void paint(QPainter *painter);

    enum START_ANGELS {
        START_E = 0,
        START_N = 90,
        START_W = 180,
        START_S = 270
    };

signals:
    void colorChanged();
    void radiusChanged();
    void radiusTopLeftChanged();
    void radiusTopRightChanged();
    void radiusBottomRightChanged();
    void radiusBottomLeftChanged();

private:
    QColor color_ = QColor(255, 255, 255);
    qreal radius_ = 0.0;
    qreal radiusTopLeft_ = -1;
    qreal radiusTopRight_ = -1;
    qreal radiusBottomRight_ = -1;
    qreal radiusBottomLeft_ = -1;

private slots:
    void onAppearanceChanged();
};

}
}
}
