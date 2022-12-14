/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtWidgets module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSTYLEANIMATION
#define QSTYLEANIMATION

#include "qabstractanimation.h"
#include "qdatetime.h"
#include "qimage.h"

namespace dstyle {

#ifndef QT_NO_ANIMATION

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists for the convenience of
// qcommonstyle.cpp.  This header file may change from version to version
// without notice, or even be removed.
//
// We mean it.
//

class DStyleAnimation : public QAbstractAnimation
{
    Q_OBJECT

public:
    DStyleAnimation(QObject *target);
    virtual ~DStyleAnimation();

    QObject *target() const;

    int duration() const Q_DECL_OVERRIDE;
    void setDuration(int duration);

    int delay() const;
    void setDelay(int delay);

    QTime startTime() const;
    void setStartTime(const QTime &time);

    QAbstractAnimation::DeletionPolicy deletePolicy() const;
    void setDeletePolicy(QAbstractAnimation::DeletionPolicy policy);

    enum FrameRate { DefaultFps, SixtyFps, ThirtyFps, TwentyFps };

    FrameRate frameRate() const;
    void setFrameRate(FrameRate fps);

    void updateTarget();

public Q_SLOTS:
    void start();

protected:
    virtual bool isUpdateNeeded() const;
    virtual void updateCurrentTime(int time) Q_DECL_OVERRIDE;

private:
    int _delay;
    int _duration;
    QTime _startTime;
    FrameRate _fps;
    int _skip;
    int _policy = QAbstractAnimation::DeletionPolicy::DeleteWhenStopped;
};

class DProgressStyleAnimation : public DStyleAnimation
{
    Q_OBJECT

public:
    DProgressStyleAnimation(int speed, QObject *target);

    int animationStep() const;
    int progressStep(int width) const;

    int speed() const;
    void setSpeed(int speed);

protected:
    bool isUpdateNeeded() const Q_DECL_OVERRIDE;

private:
    int _speed;
    mutable int _step;
};

class DNumberStyleAnimation : public DStyleAnimation
{
    Q_OBJECT

public:
    DNumberStyleAnimation(QObject *target);

    qreal startValue() const;
    void setStartValue(qreal value);

    qreal endValue() const;
    void setEndValue(qreal value);

    qreal currentValue() const;

protected:
    bool isUpdateNeeded() const Q_DECL_OVERRIDE;

private:
    qreal _start;
    qreal _end;
    mutable qreal _prev;
};

class DBlendStyleAnimation : public DStyleAnimation
{
    Q_OBJECT

public:
    enum Type { Transition, Pulse };

    DBlendStyleAnimation(Type type, QObject *target);

    QImage startImage() const;
    void setStartImage(const QImage &image);

    QImage endImage() const;
    void setEndImage(const QImage &image);

    QImage currentImage() const;

protected:
    virtual void updateCurrentTime(int time) Q_DECL_OVERRIDE;

private:
    Type _type;
    QImage _start;
    QImage _end;
    QImage _current;
};

class DScrollbarStyleAnimation : public DNumberStyleAnimation
{
    Q_OBJECT

public:
    enum Mode { Activating, Deactivating };

    DScrollbarStyleAnimation(Mode mode, QObject *target);

    Mode mode() const;
    void restart(bool blocksig = false);

    bool wasActive() const;
    void setActive(bool active);

private:
    Mode _mode;
    bool _active;
};

#endif  // QT_NO_ANIMATION

}  // end namespace dstyle

#endif  // QSTYLEANIMATION_P
