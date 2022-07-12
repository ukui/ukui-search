#ifndef PENDINGAPPINFO_H
#define PENDINGAPPINFO_H

#include <QString>
#include <QFlags>

namespace UkuiSearch {

class PendingAppInfo
{
public:
    enum HandleType{
        Delete              = 0,
        Insert              = 1u,
        UpdateAll           = 1u << 1,
        UpdateLocaleData    = 1u << 2,
        UpdateLaunchTimes   = 1u << 3,
        UpdateFavorites     = 1u << 4,
        UpdateTop           = 1u << 5,
        UpdateLock          = 1u << 6
    };
    Q_DECLARE_FLAGS(HandleTypes, HandleType)

    PendingAppInfo() = default;
    PendingAppInfo(QString desktopfp, HandleTypes type, int favorites = -1, int top = -1, int lock = -1)
        : m_desktopfp(desktopfp),
          m_handleType(type),
          m_favoritesState(favorites),
          m_topState(top),
          m_lockState(lock) {}

    QString path() const {return m_desktopfp;}
    HandleTypes handleType() const {return m_handleType;}
    int favoritesState() const {return m_favoritesState;}
    int topState() const {return m_topState;}
    int lockState() const {return m_lockState;}

    void setHandleType(const PendingAppInfo & info) {m_handleType = info.handleType();}
    void setFavorites(int favoritesState) {m_favoritesState = favoritesState;}
    void setTop(int top) {m_topState = top;}
    void setLock(int lock) {m_lockState = lock;}
    void merge(const PendingAppInfo &info)
    {
        m_handleType |= info.handleType();

        if (info.favoritesState() != -1) {
            m_favoritesState = info.favoritesState();
        }

        if (info.topState() != -1) {
            m_topState = info.topState();
        }

        if (info.lockState() != -1) {
            m_lockState = info.lockState();
        }
    }

    bool operator ==(const PendingAppInfo& rhs) const {
        return (m_desktopfp == rhs.m_desktopfp);
    }
private:
    QString m_desktopfp;
    HandleTypes m_handleType;
    int m_favoritesState;
    int m_topState;
    int m_lockState;
};
}

Q_DECLARE_OPERATORS_FOR_FLAGS(UkuiSearch::PendingAppInfo::HandleTypes)

#endif // PENDINGAPPINFO_H
