#ifndef FLODERLISTITEM_H
#define FLODERLISTITEM_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>

class FloderListItem : public QWidget
{
    Q_OBJECT
public:
    explicit FloderListItem(QWidget *parent = nullptr, const QString &path = 0);
    ~FloderListItem();

protected:
    virtual void enterEvent(QEvent * event);
    virtual void leaveEvent(QEvent * event);
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
};

#endif // FLODERLISTITEM_H
