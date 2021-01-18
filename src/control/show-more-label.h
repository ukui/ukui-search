#ifndef SHOWMORELABEL_H
#define SHOWMORELABEL_H

#include <QWidget>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>

class ShowMoreLabel : public QWidget
{
    Q_OBJECT
public:
    explicit ShowMoreLabel(QWidget *parent = nullptr);
    ~ShowMoreLabel();

protected:
    bool eventFilter(QObject *, QEvent *);

private:
    QHBoxLayout * m_layout = nullptr;
    QLabel * m_textLabel = nullptr;
    QLabel * m_loadingIconLabel = nullptr;
    QTimer * m_timer = nullptr;
    bool m_isOpen = false;
    int m_currentState = 0;

    void initUi();

Q_SIGNALS:
    void showMoreClicked();
    void retractClicked();

public Q_SLOTS:
    void startLoading();
    void stopLoading();
    void refreshLoadState();
};

#endif // SHOWMORELABEL_H
