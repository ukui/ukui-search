#include "file-utils.h"

FileUtils::FileUtils(QWidget *parent)
    : QWidget(parent)
{
}

FileUtils::~FileUtils()
{
}

QString FileUtils::getFileIconName(const QString &uri, bool checkValid)
{
    auto file = wrapGFile(g_file_new_for_uri(uri.toUtf8().constData()));
    auto info = wrapGFileInfo(g_file_query_info(file.get()->get(),
                              G_FILE_ATTRIBUTE_STANDARD_ICON,
                              G_FILE_QUERY_INFO_NONE,
                              nullptr,
                              nullptr));
    if (!G_IS_FILE_INFO (info.get()->get()))
        return nullptr;
    GIcon *g_icon = g_file_info_get_icon (info.get()->get());
    QString icon_name;
    //do not unref the GIcon from info.
    if (G_IS_ICON(g_icon)) {
        const gchar* const* icon_names = g_themed_icon_get_names(G_THEMED_ICON (g_icon));
        if (icon_names) {
            auto p = icon_names;
            if (*p)
                icon_name = QString (*p);
            if (checkValid) {
                while (*p) {
                    QIcon icon = QIcon::fromTheme(*p);
                    if (!icon.isNull()) {
                        icon_name = QString (*p);
                        break;
                    } else {
                        p++;
                    }
                }
            }
        }
    }
    return icon_name;
}
