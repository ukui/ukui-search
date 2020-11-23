#ifndef GT_H
#define GT_H
#include <glib-object.h>
#include <gio/gio.h>
#include <memory>

template<class T>

class gobjecttemplate
{
public:
    //do not use this constructor.
    gobjecttemplate();
    gobjecttemplate(T *obj, bool ref = false) {
        m_obj = obj;
        if (ref) {
            g_object_ref(obj);
        }
    }

    ~gobjecttemplate() {
        //qDebug()<<"~GObjectTemplate";
        if (m_obj)
            g_object_unref(m_obj);
    }

    T *get() {
        return m_obj;
    }

private:
    mutable T *m_obj = nullptr;
};


std::shared_ptr<gobjecttemplate<GFile>> wrapGFile(GFile *file);
std::shared_ptr<gobjecttemplate<GFileInfo>> wrapGFileInfo(GFileInfo *info);

#endif // GT_H
