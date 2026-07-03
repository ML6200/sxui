#pragma once

#include <QVariant>
#include <QVector>
#include <QWidget>

class QAbstractItemModel;
class QLabel;
class QPushButton;
class QStandardItemModel;
class QTableView;

namespace sxui {

// Editable data grid. Ships with an internal model driven by the
// setColumns()/setRows()/appendRow() API and an optional control strip
// (ADD ROW / REMOVE, row count readout); alternatively bind any
// QAbstractItemModel via setModel(), which disables the built-in row
// editing API. Cells are edited in place (double-click or F2).
class TableEditor : public QWidget {
    Q_OBJECT
public:
    explicit TableEditor(QWidget* parent = nullptr);

    void setColumns(const QStringList& headers);
    void setRows(const QVector<QVariantList>& rows);
    QVector<QVariantList> rows() const;
    void appendRow(const QVariantList& row = {});
    void removeSelectedRows();

    int rowCount() const;
    int columnCount() const;

    void setReadOnly(bool readOnly);
    void setControlsVisible(bool visible);

    // Replaces the internal model; setColumns()/setRows()/appendRow()
    // no longer apply and the control strip is hidden.
    void setModel(QAbstractItemModel* model);

    // Escape hatch for view-level tuning (column widths, sorting, ...).
    QTableView* view() const { return m_view; }

signals:
    void cellEdited(int row, int column, const QVariant& value);
    void rowCountChanged(int count);

private:
    void updateRowCount();

    QStandardItemModel* m_model;
    QTableView* m_view;
    QWidget* m_controls;
    QPushButton* m_addButton;
    QPushButton* m_removeButton;
    QLabel* m_countLabel;
    bool m_readOnly = false;
    bool m_externalModel = false;
};

} // namespace sxui
