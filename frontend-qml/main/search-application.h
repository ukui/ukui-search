//
// Created by hxf on 22-7-1.
//

#ifndef UKUI_SEARCH_SEARCH_APPLICATION_H
#define UKUI_SEARCH_SEARCH_APPLICATION_H

#include "qtsingleapplication.h"

namespace UkuiSearch {
class MainWindow;
}

class SearchApplication : public QtSingleApplication
{
    Q_OBJECT
public:
    explicit SearchApplication(int &argc, char **argv, int = ApplicationFlags);

    ~SearchApplication() override;

private:
    void initMainWindow();
    void parseCommand(const QString &msg, bool isPrimary);
    static void loadTranslation();

private:
    UkuiSearch::MainWindow *m_mainWindow = nullptr;
};

#endif //UKUI_SEARCH_SEARCH_APPLICATION_H
