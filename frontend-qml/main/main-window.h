//
// Created by hxf on 22-5-20.
//

#ifndef UKUI_SEARCH_MAIN_WINDOW_H
#define UKUI_SEARCH_MAIN_WINDOW_H

#include <QObject>
#include <QPushButton>

namespace UkuiSearch {

class LineEdit;
class ResultView;
class SearchWorkerManager;

class MainWindow : public QObject
{
    Q_OBJECT
public:
    explicit MainWindow();

public Q_SLOTS:
    void showWindow();
    void hideWindow();
    void showLineEdit();
    void showResultView();

protected:

private:
    void initUI();
    void initManager();
    void initConnections();

private Q_SLOTS:
    void checkFocus(QWindow *focusWindow);

private:
    LineEdit *m_lineEdit = nullptr;
    ResultView *m_resultView = nullptr;

    SearchWorkerManager *m_workManager = nullptr;
};

} // UkuiSearch

#endif //UKUI_SEARCH_MAIN_WINDOW_H
