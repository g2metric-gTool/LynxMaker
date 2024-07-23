#pragma once

#include <QPainter>
#include <QQuickItem>
#include <QQuickPaintedItem>
#include <QPropertyAnimation>

#include "system/xStdafx.h"

class CircularReveal : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY_AUTO(QQuickItem*, target)
    Q_PROPERTY_AUTO(int, radius)
public:
    CircularReveal(QQuickItem* parent = nullptr);
    void paint(QPainter* painter) override;
    Q_INVOKABLE void start(int w, int h, const QPoint& center, int radius);
    Q_SIGNAL void imageChanged();
    Q_SIGNAL void animationFinished();
    Q_SLOT void handleGrabResult();
private:
    QPropertyAnimation* _anim = nullptr;
    QImage _source;
    QPoint _center;
    QSharedPointer<QQuickItemGrabResult>  _grabResult;
};

