#include "home-page-item.h"

HomePageItem::HomePageItem(QWidget *parent, const int& type, const QString& path) : QWidget(parent)
{
    setupUi(type, path);
}

HomePageItem::~HomePageItem()
{
}

/**
 * @brief HomePageItem::setupUi 根据不同的分栏创建item
 * @param type 所属分栏
 * @param path 路径
 */
void HomePageItem::setupUi(const int& type, const QString& path) {
    m_widget = new QWidget(this);
    m_widget->setObjectName("MainWidget");
    m_widget->setStyleSheet("QWidget#MainWidget{background: rgba(0, 0, 0, 0.1); border-radius: 4px;}");
    m_iconlabel = new QLabel(m_widget);
    m_namelabel = new QLabel(m_widget);
    if (type == ItemType::Recent) {
        m_widget->setFixedSize(265, 48);
        QIcon icon;
        switch (SearchListView::getResType(path)) { //可能出现文件应用等，需要根据路径判断类型
            case SearchListView::ResType::App : {
                icon = FileUtils::getAppIcon(path);
                m_namelabel->setText(FileUtils::getAppName(path));
                break;
            }
            case SearchListView::ResType::File : {
                icon = FileUtils::getFileIcon(QString("file://%1").arg(path));
                m_namelabel->setText(FileUtils::getFileName(path));
                break;
            }
            case SearchListView::ResType::Setting : {
                icon = FileUtils::getSettingIcon(path, true);
                m_namelabel->setText(FileUtils::getSettingName(path));
                break;
            }
            case SearchListView::ResType::Dir : {
                break;
            }
            default :
                break;
        }
        m_iconlabel->setPixmap(icon.pixmap(icon.actualSize(QSize(24, 24))));
        m_hlayout = new QHBoxLayout(m_widget);
        m_iconlabel->setAlignment(Qt::AlignCenter);
        m_namelabel->setAlignment(Qt::AlignCenter);
        m_hlayout->addWidget(m_iconlabel);
        m_hlayout->addWidget(m_namelabel);
        m_hlayout->addStretch();
        return;
    }
    m_widget->setFixedSize(120, 120);
    m_vlayout = new QVBoxLayout(m_widget);
    m_vlayout->setContentsMargins(0,16,0,12);
    m_iconlabel->setAlignment(Qt::AlignCenter);
    m_namelabel->setAlignment(Qt::AlignCenter);
    m_vlayout->addWidget(m_iconlabel);
    m_vlayout->addWidget(m_namelabel);
    QIcon icon = FileUtils::getAppIcon(path);
    m_iconlabel->setPixmap(icon.pixmap(icon.actualSize(QSize(48, 48))));
    m_namelabel->setText(FileUtils::getAppName(path));
}
