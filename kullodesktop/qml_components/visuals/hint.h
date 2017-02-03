/* Copyright 2013–2017 Kullo GmbH. All rights reserved. */
#pragma once

#include <QQuickPaintedItem>
#include <QPainter>
#include <QPen>

namespace KulloDesktop {
namespace QmlComponents {
namespace Visuals {

class Hint : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit Hint(QQuickItem *parent = 0);

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    QColor color() const;
    void setColor(QColor color);

    Q_PROPERTY(QString direction READ direction WRITE setDirection NOTIFY directionChanged)
    QString direction() const;
    void setDirection(const QString &direction);

    Q_PROPERTY(quint32 radius READ radius WRITE setRadius NOTIFY radiusChanged)
    quint32 radius() const;
    void setRadius(quint32 r);

    Q_PROPERTY(qreal paddingTop READ paddingTop WRITE setPaddingTop NOTIFY paddingTopChanged)
    qreal paddingTop() const;
    void setPaddingTop(qreal p);

    Q_PROPERTY(qreal paddingBottom READ paddingBottom WRITE setPaddingBottom NOTIFY paddingBottomChanged)
    qreal paddingBottom() const;
    void setPaddingBottom(qreal p);

    void paint(QPainter *painter);

    enum START_ANGELS {
        START_E = 0,
        START_N = 90,
        START_W = 180,
        START_S = 270
    };

signals:
    void colorChanged();
    void directionChanged();
    void radiusChanged();
    void paddingTopChanged();
    void paddingBottomChanged();

private:
    QColor color_ = QColor(255, 255, 255);
    quint32 radius_ = 4;
    qreal paddingTop_ = 0.0;
    qreal paddingBottom_ = 0.0;
    QString direction_;

private slots:
    void onAppearanceChanged();
};

}
}
}
