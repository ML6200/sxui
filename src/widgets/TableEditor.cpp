#include "widgets/TableEditor.h"
#include "core/Theme.h"

#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPushButton>
#include <QSet>
#include <QStandardItemModel>
#include <QTableView>
#include <QVBoxLayout>
#include <QOpenGLWidget>

#include <algorithm>

namespace sxui {

TableEditor::TableEditor(QWidget* parent)
    : QWidget(parent)
    , m_model(new QStandardItemModel(this))
{
    const Theme& t = Theme::current();

    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(8);

    m_view = new QTableView;
    m_view->setViewport(new QOpenGLWidget());
    m_view->setModel(m_model);
    m_view->setAlternatingRowColors(true);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_view->setFrameShape(QFrame::Box);

    QHeaderView* hh = m_view->horizontalHeader();
    hh->setStretchLastSection(true);
    hh->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    hh->setSectionsClickable(false);
    hh->setFont(t.headingFont(8.5));
    hh->setFixedHeight(30);

    QHeaderView* vh = m_view->verticalHeader();
    vh->setDefaultSectionSize(30);
    vh->setSectionResizeMode(QHeaderView::Fixed);
    vh->setFont(t.font(8.0));
    vh->setDefaultAlignment(Qt::AlignCenter);
    vh->setMinimumWidth(32);

    lay->addWidget(m_view, 1);

    m_controls = new QWidget;
    auto* cl = new QHBoxLayout(m_controls);
    cl->setContentsMargins(0, 0, 0, 0);
    cl->setSpacing(8);
    m_addButton = new QPushButton(QStringLiteral("ADD ROW"));
    m_removeButton = new QPushButton(QStringLiteral("REMOVE"));
    cl->addWidget(m_addButton);
    cl->addWidget(m_removeButton);
    cl->addStretch();
    m_countLabel = new QLabel;
    m_countLabel->setFont(t.font(8.5));
    QPalette pal = m_countLabel->palette();
    pal.setColor(QPalette::WindowText, t.textDim);
    m_countLabel->setPalette(pal);
    cl->addWidget(m_countLabel);
    lay->addWidget(m_controls);

    connect(m_addButton, &QPushButton::clicked, this, [this] { appendRow(); });
    connect(m_removeButton, &QPushButton::clicked, this, &TableEditor::removeSelectedRows);
    connect(m_model, &QStandardItemModel::itemChanged, this, [this](QStandardItem* item) {
        emit cellEdited(item->row(), item->column(), item->data(Qt::EditRole));
    });
    connect(m_model, &QStandardItemModel::rowsInserted, this, &TableEditor::updateRowCount);
    connect(m_model, &QStandardItemModel::rowsRemoved, this, &TableEditor::updateRowCount);
    updateRowCount();
}

void TableEditor::setColumns(const QStringList& headers)
{
    if (m_externalModel)
        return;
    m_model->setColumnCount(headers.size());
    m_model->setHorizontalHeaderLabels(headers);
}

void TableEditor::setRows(const QVector<QVariantList>& rows)
{
    if (m_externalModel)
        return;
    m_model->removeRows(0, m_model->rowCount());
    for (const QVariantList& row : rows)
        appendRow(row);
    // Size columns to their content plus breathing room; the last column
    // stretches to fill the remainder.
    m_view->resizeColumnsToContents();
    for (int c = 0; c < m_model->columnCount() - 1; ++c)
        m_view->setColumnWidth(c, m_view->columnWidth(c) + 18);
}

QVector<QVariantList> TableEditor::rows() const
{
    QVector<QVariantList> out;
    if (m_externalModel)
        return out;
    out.reserve(m_model->rowCount());
    for (int r = 0; r < m_model->rowCount(); ++r) {
        QVariantList row;
        row.reserve(m_model->columnCount());
        for (int c = 0; c < m_model->columnCount(); ++c) {
            QStandardItem* item = m_model->item(r, c);
            row.append(item ? item->data(Qt::EditRole) : QVariant());
        }
        out.append(row);
    }
    return out;
}

void TableEditor::appendRow(const QVariantList& row)
{
    if (m_externalModel)
        return;
    QList<QStandardItem*> items;
    const int columns = qMax(m_model->columnCount(), 1);
    for (int c = 0; c < columns; ++c) {
        auto* item = new QStandardItem;
        if (c < row.size())
            item->setData(row[c], Qt::EditRole);
        items.append(item);
    }
    m_model->appendRow(items);
}

void TableEditor::removeSelectedRows()
{
    if (m_externalModel || !m_view->selectionModel())
        return;
    QSet<int> rowSet;
    const QModelIndexList selection = m_view->selectionModel()->selectedIndexes();
    for (const QModelIndex& index : selection)
        rowSet.insert(index.row());
    QList<int> sorted(rowSet.begin(), rowSet.end());
    std::sort(sorted.begin(), sorted.end(), std::greater<int>());
    for (int row : sorted)
        m_model->removeRow(row);
}

int TableEditor::rowCount() const
{
    return m_view->model() ? m_view->model()->rowCount() : 0;
}

int TableEditor::columnCount() const
{
    return m_view->model() ? m_view->model()->columnCount() : 0;
}

void TableEditor::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
    m_view->setEditTriggers(readOnly ? QAbstractItemView::NoEditTriggers
                                     : QAbstractItemView::DoubleClicked
                                           | QAbstractItemView::EditKeyPressed
                                           | QAbstractItemView::SelectedClicked);
    m_addButton->setEnabled(!readOnly);
    m_removeButton->setEnabled(!readOnly);
}

void TableEditor::setControlsVisible(bool visible)
{
    m_controls->setVisible(visible);
}

void TableEditor::setModel(QAbstractItemModel* model)
{
    m_externalModel = true;
    m_view->setModel(model);
    setControlsVisible(false);
    if (model) {
        connect(model, &QAbstractItemModel::rowsInserted, this, &TableEditor::updateRowCount);
        connect(model, &QAbstractItemModel::rowsRemoved, this, &TableEditor::updateRowCount);
    }
    updateRowCount();
}

void TableEditor::updateRowCount()
{
    m_countLabel->setText(QStringLiteral("ROWS: %1").arg(rowCount()));
    emit rowCountChanged(rowCount());
}

} // namespace sxui
