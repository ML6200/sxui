#pragma once

#include <QWidget>

class QFileSystemModel;
class QLineEdit;
class QListView;
class QModelIndex;
class QPushButton;

namespace sxui {

// Filesystem browser rendered as Silo-style entry cards: outlined icon,
// name, date line in amber, size at the right edge. Double-clicking a
// directory descends into it; files emit fileActivated().
class FileBrowser : public QWidget {
    Q_OBJECT
public:
    explicit FileBrowser(QWidget* parent = nullptr);

    QString path() const;
    void setPath(const QString& path);
    // e.g. {"*.png", "*.jpg"}; non-matching files are hidden.
    void setNameFilters(const QStringList& filters);

signals:
    void fileActivated(const QString& path);
    void pathChanged(const QString& path);

private:
    void activate(const QModelIndex& index);
    void goUp();

    QFileSystemModel* m_model;
    QListView* m_view;
    QLineEdit* m_pathEdit;
    QPushButton* m_upButton;
};

} // namespace sxui
