#include "home-page-section.h"

#define SQUARE_WIDTH 116
#define SQUARE_HEIGHT 116
#define SQUARE_ICON_SIZE 48
#define SQUARE_NAME_SIZE 108
#define BAR_WIDTH 300
#define BAR_HEIGHT 48
#define BAR_ICON_SIZE 24
#define BAR_NAME_SIZE 250
#define TITLE_HEIGHT 24
#define MAIN_LAYOUT_SPACING 6
#define ITEM_LAYOUT_SPACING 8
#define ITEM_RADIUS 4
#define ITEM_TRANS_NORMAL 0.06
#define ITEM_TRANS_HOVER 0.15
#define ITEM_TRANS_PRESS 0.06
#define ITEM_MARGINS 0,0,0,0
#define FLOW_LAYOUT_MARGIN 0

using namespace Zeeker;

HomePageSection::HomePageSection(QString title, HomePageItemShape shape, QWidget *parent) : QWidget(parent)
{
    m_title = title;
    m_shape = shape;
    initUi();
}

HomePageSection::~HomePageSection()
{
    if (m_itemsLyt) {
        delete m_itemsLyt;
        m_itemsLyt = NULL;
    }
}

/**
 * @brief HomePageSection::setItems 设置所有的项
 * @param itemList 显示的Item列表
 */
void HomePageSection::setItems(QVector<HomePageItem> itemList)
{
    clear();
    if (m_shape == HomePageItemShape::Square) {
        Q_FOREACH (HomePageItem item, itemList) {
            m_itemsLyt->addWidget(createSquareItem(item));
        }
        int line_num = ceil(double(itemList.length())/5);
        this->setFixedHeight(TITLE_HEIGHT + MAIN_LAYOUT_SPACING + ITEM_LAYOUT_SPACING*(line_num-1) + SQUARE_HEIGHT*line_num);
    } else {
        Q_FOREACH (HomePageItem item, itemList) {
            m_itemsLyt->addWidget(createBarItem(item));
        }
        int line_num = ceil(double(itemList.length())/2);
        this->setFixedHeight(TITLE_HEIGHT + MAIN_LAYOUT_SPACING + ITEM_LAYOUT_SPACING*(line_num-1) + BAR_HEIGHT*line_num);
    }
    m_length = itemList.length();
}

int HomePageSection::length()
{
    return m_length;
}

///**
// * @brief HomePageSection::appendItem 添加一项
// * @param item 要添加的item
// */
//void HomePageSection::appendItem(HomePageItem item)
//{
//    if (m_shape == HomePageItemShape::Square) {
//        m_itemsLyt->addWidget(createSquareItem(item));
//    } else {
//        m_itemsLyt->addWidget(createBarItem(item));
//    }
//    m_length++;
//}

//NEW_TODO
///**
// * @brief HomePageSection::insertItem 插入一项
// * @param index 插入位置
// * @param item 要插入的item
// */
//void HomePageSection::insertItem(const int &index, const HomePageSection::HomePageItem &item)
//{

//}

///**
// * @brief HomePageSection::removeOne 删除一项
// * @param key HomePageSection的key，用来寻找item
// */
//void HomePageSection::removeOne(const QString &key)
//{
//    Q_FOREACH (ItemWidget * item, m_itemsLyt->findChildren<ItemWidget *>()) {
//        if (item->objectName() == key) {
//            delete item;
//            item = NULL;
//            break;
//        }
//    }
//}

/**
 * @brief HomePageSection::clear 清空
 */
void HomePageSection::clear()
{
    if(! m_itemsLyt) return;
    QLayoutItem * item;
    while((item = m_itemsLyt->takeAt(0)) != 0) {
        if(item->widget()) {
            item->widget()->setParent(NULL); //防止删除后窗口看上去没有消失
        }
        delete item;
    }
    item = NULL;
    this->setFixedHeight(0);
    m_length = 0;
}

/**
 * @brief HomePageSection::initUi 初始化固有控件
 */
void HomePageSection::initUi()
{
    m_mainLyt = new QVBoxLayout(this);
    m_mainLyt->setContentsMargins(ITEM_MARGINS);
    this->setLayout(m_mainLyt);
    m_titleLabel = new QLabel(this);
    m_titleLabel->setFixedHeight(TITLE_HEIGHT);
    m_titleLabel->setText(m_title);
    m_itemWidget = new QWidget(this);
    m_mainLyt->setSpacing(MAIN_LAYOUT_SPACING);
    m_itemsLyt = new FlowLayout(m_itemWidget, FLOW_LAYOUT_MARGIN, ITEM_LAYOUT_SPACING, ITEM_LAYOUT_SPACING);
    m_itemWidget->setLayout(m_itemsLyt);
    m_mainLyt->addWidget(m_titleLabel);
    m_mainLyt->addWidget(m_itemWidget);
}

///**
// * @brief HomePageSection::resize 重新计算此窗口size
// */
//void HomePageSection::resize()
//{

//}

/**
 * @brief HomePageSection::createSquareItem 创建一个方形小窗格
 * @param item 数据
 * @return
 */
ItemWidget *HomePageSection::createSquareItem(const HomePageItem &item)
{
    ItemWidget * square_item = new ItemWidget(m_itemWidget, HomePageItemShape::Square, item);
    square_item->setObjectName(item.key);
    connect(square_item, &ItemWidget::clicked, this, &HomePageSection::requestAction);
    return square_item;
}

/**
 * @brief HomePageSection::createBarItem 创建一个条状小窗格
 * @param item 数据
 * @return
 */
ItemWidget *HomePageSection::createBarItem(const HomePageItem &item)
{
    ItemWidget * bar_item = new ItemWidget(m_itemWidget, HomePageItemShape::Bar, item);
    bar_item->setObjectName(item.key);
    connect(bar_item, &ItemWidget::clicked, this, &HomePageSection::requestAction);
    return bar_item;
}

/**
 * @brief ItemWidget::ItemWidget 显示在flowlayout中的每个小窗格
 * @param parent 父窗口
 * @param shape 形状
 * @param item 数据
 */
ItemWidget::ItemWidget(QWidget *parent, const HomePageItemShape &shape, const HomePageItem &item) : QWidget(parent) {
    this->initUi(shape, item.name, item.icon);
    m_item = item;
    this->installEventFilter(this);
    this->setToolTip(item.name);
    m_transparency = ITEM_TRANS_NORMAL;
    connect(qApp, &QApplication::paletteChanged, this, [ = ]() {
        if(m_namelabel) {
            QString name = this->toolTip();
            if(shape == HomePageItemShape::Square) {
                m_namelabel->setText(m_namelabel->fontMetrics().elidedText(name, Qt::ElideRight, SQUARE_NAME_SIZE));
            } else {
                m_namelabel->setText(m_namelabel->fontMetrics().elidedText(name, Qt::ElideRight, BAR_NAME_SIZE));
            }
        }
    });
}

/**
 * @brief HomePageItem::setupUi 根据不同的形状创建item
 * @param shape 形状
 * @param name 名称
 * @param icon 图标
 */
void ItemWidget::initUi(HomePageItemShape shape, const QString& name, const QIcon& icon) {
    m_iconlabel = new QLabel(this);
    m_namelabel = new QLabel(this);
    m_iconlabel->setAlignment(Qt::AlignCenter);
    m_namelabel->setAlignment(Qt::AlignCenter);
    if (shape == HomePageItemShape::Square) {
        m_vlayout = new QVBoxLayout(this);
        this->setLayout(m_vlayout);
        m_iconlabel->setPixmap(icon.pixmap(icon.actualSize(QSize(SQUARE_ICON_SIZE, SQUARE_ICON_SIZE))));
        m_namelabel->setText(m_namelabel->fontMetrics().elidedText(name, Qt::ElideRight, SQUARE_NAME_SIZE));
        m_vlayout->addWidget(m_iconlabel);
        m_vlayout->addWidget(m_namelabel);
        this->setFixedSize(SQUARE_WIDTH, SQUARE_HEIGHT);
    } else {
        m_hlayout = new QHBoxLayout(this);
        this->setLayout(m_hlayout);
        m_iconlabel->setPixmap(icon.pixmap(icon.actualSize(QSize(BAR_ICON_SIZE, BAR_ICON_SIZE))));
        m_namelabel->setText(m_namelabel->fontMetrics().elidedText(name, Qt::ElideRight, BAR_NAME_SIZE));
        m_hlayout->addWidget(m_iconlabel);
        m_hlayout->addWidget(m_namelabel);
        m_hlayout->addStretch();
        this->setFixedSize(BAR_WIDTH, BAR_HEIGHT);
    }
}

bool ItemWidget::eventFilter(QObject *watched, QEvent *event) {
    if(watched == this) {
        if(event->type() == QEvent::MouseButtonPress) {
            m_transparency = ITEM_TRANS_PRESS;
            this->update();
            return true;
        } else if(event->type() == QEvent::MouseButtonRelease) {
            Q_EMIT this->clicked(m_item.key, m_item.action, m_item.pluginId);
            m_transparency = ITEM_TRANS_NORMAL;
            this->update();
            return true;
        } else if(event->type() == QEvent::Enter) {
            m_transparency = ITEM_TRANS_HOVER;
            this->update();
            return true;
        } else if(event->type() == QEvent::Leave) {
            m_transparency = ITEM_TRANS_NORMAL;
            this->update();
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void ItemWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event)
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QRect rect = this->rect();
    p.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
    p.setBrush(opt.palette.color(QPalette::Text));
    p.setOpacity(m_transparency);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, ITEM_RADIUS, ITEM_RADIUS);
    return QWidget::paintEvent(event);
}

