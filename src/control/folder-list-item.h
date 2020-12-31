#ifndef FOLDERLISTITEM_H
#define FOLDERLISTITEM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

class FolderListItem : public QWidget
{
    Q_OBJECT
public:
    explicit FolderListItem(QWidget *parent = nullptr, const QString &path = 0);
    ~FolderListItem();
    QString getPath();

protected:
    virtual void enterEvent(QEvent *);
    virtual void leaveEvent(QEvent *);
    bool eventFilter(QObject *, QEvent *);

private:
    void initUi();

    QString m_path;

    QVBoxLayout * m_layout = nullptr;
    QWidget * m_widget = nullptr;
    QHBoxLayout * m_widgetlayout = nullptr;
    QLabel * m_iconLabel = nullptr;
    QLabel * m_pathLabel = nullptr;
    QLabel * m_delLabel = nullptr;
Q_SIGNALS:
    void onDelBtnClicked(const QString&);
};

#endif // FOLDERLISTITEM_H
