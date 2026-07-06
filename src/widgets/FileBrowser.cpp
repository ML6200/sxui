#include "widgets/FileBrowser.h"
#include "core/Theme.h"

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QListView>
#include <QPainter>
#include <QPushButton>
#include <QStyledItemDelegate>
#include <QVBoxLayout>
#include <QOpenGLWidget>

namespace sxui {

namespace {

QString formatSize(qint64 bytes)
{
    if (bytes < 1024)
        return QString::number(bytes) + QStringLiteral("B");
    if (bytes < 1024 * 1024)
        return QString::number(bytes / 1024.0, 'f', 0) + QStringLiteral("KB");
    if (bytes < qint64(1024) * 1024 * 1024)
        return QString::number(bytes / (1024.0 * 1024.0), 'f', 1) + QStringLiteral("MB");
    return QString::number(bytes / (1024.0 * 1024.0 * 1024.0), 'f', 1) + QStringLiteral("GB");
}

QString formatDate(const QDateTime& dt)
{
    return QLocale::c().toString(dt, QStringLiteral("MMM dd, yyyy")).toUpper();
}

// Document glyph as in the show's archive listing: outer box with a small
// inset rectangle in the upper half.
void drawFileIcon(QPainter* p, const QRectF& r, const QColor& color)
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing, false);
    p->setPen(QPen(color, 1.5));
    p->setBrush(Qt::NoBrush);
    const QRectF box = r.adjusted(3, 1, -3, -1);
    p->drawRect(box);
    const qreal w = box.width() * 0.5;
    p->drawRect(QRectF(box.center().x() - w / 2, box.y() + box.height() * 0.2,
                       w, box.height() * 0.28));
    p->restore();
}

void drawFolderIcon(QPainter* p, const QRectF& r, const QColor& color)
{
    p->save();
    p->setRenderHint(QPainter::Antialiasing, false);
    p->setPen(QPen(color, 1.5));
    p->setBrush(Qt::NoBrush);
    const QRectF box = r.adjusted(1, 4, -1, -3);
    p->drawRect(box);
    // Tab
    p->drawLine(QPointF(box.x(), box.y() - 3), QPointF(box.x() + box.width() * 0.45, box.y() - 3));
    p->drawLine(QPointF(box.x(), box.y() - 3), QPointF(box.x(), box.y()));
    p->drawLine(QPointF(box.x() + box.width() * 0.45, box.y() - 3),
                QPointF(box.x() + box.width() * 0.45, box.y()));
    p->restore();
}

class FileCardDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override
    {
        return { 220, 54 };
    }

    void paint(QPainter* p, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override
    {
        const Theme& t = Theme::current();
        const auto* model = qobject_cast<const QFileSystemModel*>(index.model());
        if (!model)
            return;
        const QFileInfo info = model->fileInfo(index);

        const bool selected = option.state & QStyle::State_Selected;
        const bool hover = option.state & QStyle::State_MouseOver;
        const QRectF card = QRectF(option.rect).adjusted(1.5, 1.5, -1.5, -1.5);

        QColor border = t.border;
        QColor fill = t.surface;
        if (selected) {
            border = t.primary;
            fill = alpha(t.primary, 26);
        } else if (hover) {
            border = t.borderBright;
        }
        p->fillRect(card, fill);
        p->setPen(QPen(border, 1.0));
        p->setBrush(Qt::NoBrush);
        p->drawRect(card);

        // Icon
        const QRectF iconRect(card.x() + 12, card.center().y() - 12, 24, 24);
        const QColor iconColor = alpha(t.warning, info.isDir() ? 140 : 190);
        if (info.isDir())
            drawFolderIcon(p, iconRect, iconColor);
        else
            drawFileIcon(p, iconRect, iconColor);

        const QFontMetricsF sizeFm(t.font(8.5));
        const qreal sizeW = sizeFm.horizontalAdvance(QStringLiteral("0000.0MB")) + 8;
        const qreal textX = iconRect.right() + 14;
        const qreal textW = card.right() - 12 - sizeW - textX;

        // Name
        QFont nameFont = t.headingFont(9.5, QFont::DemiBold);
        const QFontMetricsF nameFm(nameFont);
        p->setFont(nameFont);
        p->setPen(info.isDir() ? t.primary : t.textBright);
        const QString name = nameFm.elidedText(info.fileName(), Qt::ElideMiddle, textW);
        p->drawText(QRectF(textX, card.y() + 8, textW, nameFm.height()),
                    Qt::AlignLeft | Qt::AlignVCenter, name);

        // Date line, amber like the archive listing
        p->setFont(t.font(8.0));
        p->setPen(alpha(t.warning, 170));
        p->drawText(QRectF(textX, card.bottom() - 8 - sizeFm.height(), textW, sizeFm.height()),
                    Qt::AlignLeft | Qt::AlignVCenter, formatDate(info.lastModified()));

        // Size / DIR marker
        p->setFont(t.font(8.5));
        const QRectF sizeRect(card.right() - 12 - sizeW, card.y(), sizeW, card.height());
        if (info.isDir()) {
            p->setPen(t.textDim);
            p->drawText(sizeRect, Qt::AlignRight | Qt::AlignVCenter, QStringLiteral("DIR"));
        } else {
            p->setPen(alpha(t.warning, 190));
            p->drawText(sizeRect, Qt::AlignRight | Qt::AlignVCenter, formatSize(info.size()));
        }
    }
};

} // namespace

FileBrowser::FileBrowser(QWidget* parent)
    : QWidget(parent)
    , m_model(new QFileSystemModel(this))
{
    m_model->setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    m_model->setNameFilterDisables(false);

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(8);

    auto* bar = new QHBoxLayout;
    bar->setSpacing(8);
    m_upButton = new QPushButton(QStringLiteral("UP"));
    m_upButton->setFixedWidth(52);
    connect(m_upButton, &QPushButton::clicked, this, &FileBrowser::goUp);
    bar->addWidget(m_upButton);
    m_pathEdit = new QLineEdit;
    connect(m_pathEdit, &QLineEdit::returnPressed, this,
            [this] { setPath(m_pathEdit->text()); });
    bar->addWidget(m_pathEdit, 1);
    lay->addLayout(bar);

    m_view = new QListView;
    m_view->setViewport(new QOpenGLWidget());
    m_view->setModel(m_model);
    m_view->setItemDelegate(new FileCardDelegate(m_view));
    m_view->setFrameShape(QFrame::NoFrame);
    m_view->setSpacing(3);
    m_view->setUniformItemSizes(true);
    m_view->setSelectionMode(QAbstractItemView::SingleSelection);
    m_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_view->setMouseTracking(true);
    // Cards are surface-colored; keep the list backdrop one level darker.
    QPalette pal = m_view->palette();
    pal.setColor(QPalette::Base, Theme::current().background);
    m_view->setPalette(pal);
    connect(m_view, &QListView::activated, this, &FileBrowser::activate);
    lay->addWidget(m_view, 1);

    setPath(QDir::homePath());
}

QString FileBrowser::path() const
{
    return m_model->rootPath();
}

void FileBrowser::setPath(const QString& path)
{
    const QFileInfo info(path);
    if (!info.isDir())
        return;
    const QString clean = QDir(path).absolutePath();
    m_model->setRootPath(clean);
    m_view->setRootIndex(m_model->index(clean));
    m_model->sort(0);
    m_pathEdit->setText(clean);
    emit pathChanged(clean);
}

void FileBrowser::setNameFilters(const QStringList& filters)
{
    m_model->setNameFilters(filters);
}

void FileBrowser::activate(const QModelIndex& index)
{
    const QFileInfo info = m_model->fileInfo(index);
    if (info.isDir())
        setPath(info.absoluteFilePath());
    else
        emit fileActivated(info.absoluteFilePath());
}

void FileBrowser::goUp()
{
    QDir dir(path());
    if (dir.cdUp())
        setPath(dir.absolutePath());
}

} // namespace sxui
