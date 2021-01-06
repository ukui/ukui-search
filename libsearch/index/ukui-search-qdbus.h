#ifndef UKUISEARCHQDBUS_H
#define UKUISEARCHQDBUS_H

#include <QDBusInterface>

class UkuiSearchQDBus
{
public:
    UkuiSearchQDBus();
    ~UkuiSearchQDBus();
    void setInotifyMaxUserWatches();
private:
    QDBusInterface* tmpSystemQDBusInterface;
};

#endif // UKUISEARCHQDBUS_H
