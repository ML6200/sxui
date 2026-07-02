#pragma once

#include <QAbstractButton>

class QVariantAnimation;

namespace sxui {

// Animated on/off switch. Checkable by construction; use like a QCheckBox
// (toggled(bool) signal).
class Toggle : public QAbstractButton {
    Q_OBJECT
public:
    explicit Toggle(QWidget* parent = nullptr);

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void checkStateSet() override;
    void nextCheckState() override;

private:
    void animateTo(qreal target);

    qreal m_pos = 0.0; // 0 = off, 1 = on
    QVariantAnimation* m_anim;
};

} // namespace sxui
