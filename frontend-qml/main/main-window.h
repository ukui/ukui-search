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
class ResultModelManager;

class MainWindow : public QObject
{
    Q_OBJECT
public:
    explicit MainWindow();

public Q_SLOTS:
    void showWindow();

protected:

private:
    void initUI();
    void initManager();
    void initConnections();

private Q_SLOTS:

private:
    LineEdit *m_lineEdit = nullptr;
    ResultView *m_resultView = nullptr;
    QPushButton *m_button = nullptr;

    SearchWorkerManager *m_workManager = nullptr;
    ResultModelManager *m_modelManager = nullptr;
};

} // UkuiSearch

#endif //UKUI_SEARCH_MAIN_WINDOW_H
