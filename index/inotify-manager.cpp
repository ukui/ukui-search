#include "inotify-manager.h"
#include "index-generator.h"
#include "messagelistmanager.h"

bool InotifyManager::Traverse_BFS(const QString& path, const bool& CREATORDELETE){
    qDebug() << "BFS start-----------------------------";
    int total = 0;
    MessageListManager mlm;
    mlm.SetAutoSendMessageLength(80000);
    QQueue<QString> bfs;
    bfs.enqueue(path);
    QFileInfoList list;
    QDir dir;
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    while (!bfs.empty()) {
        dir.setPath(bfs.dequeue());
        list = dir.entryInfoList();
        for (auto i : list){
//            qDebug() << i.absoluteFilePath();
            if (i.isDir()){
                AddWatch(i.absoluteFilePath());
                bfs.enqueue(i.absoluteFilePath());
            }
            else{
                mlm.AddMessage(i.absoluteFilePath());
                 total++;
                //continue;
            }
        }
    }
    mlm.SendMessage();
    qDebug()<<total;
    qDebug() << "BFS end-----------------------------";
    return true;
}

bool InotifyManager::Traverse(const QString& path, const bool& CREATORDELETE){

    QDir dir(path);
    if (!dir.exists()) {
        return false;
    }
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    int i = 0;
    do {
        if(list.size()==0){
            break;
        }
        QFileInfo fileInfo = list.at(i);
        bool bisDir = fileInfo.isDir();
        if (fileInfo.fileName().at(0) != '.'){
            if (bisDir) {
                //                qDebug() << QString("Path: %0/%1")
                //                         .arg(fileInfo.path())
                //                         .arg(fileInfo.fileName());
                qDebug() << "inotify-manager traverse: " << fileInfo.filePath();
                if (CREATORDELETE){
                    AddWatch(fileInfo.filePath());
                }
                Traverse(fileInfo.filePath(), CREATORDELETE);
                if (!CREATORDELETE){
                    RemoveWatch(fileInfo.filePath());
                }
            } else {
                //                qDebug() << QString("File: %0/%1")
                //                         .arg(fileInfo.path())
                //                         .arg(fileInfo.fileName());
                //IndexGenerator::getInstance()->creatAllIndex(new QStringList(fileInfo.filePath()));
            }
        }
        i++;
    } while (i < list.size());
    return true;
}

bool InotifyManager::AddWatch(const QString &path){
    //m_fd = inotify_init();
//    qDebug() << "m_fd: " <<m_fd;
    //int ret = inotify_add_watch(m_fd, path.toStdString().c_str(), IN_ALL_EVENTS);
    int ret = inotify_add_watch(m_fd, path.toStdString().c_str(), (IN_MOVED_FROM | IN_MOVED_TO | IN_CREATE | IN_DELETE));
    if (ret == -1) {
        return false;
    }
    currentPath[ret] = path;
//    qDebug() << "Watch:" << path;
    return true;
}

//暂时没用
bool InotifyManager::AddWatchList(const QStringList &paths){
    m_fd = inotify_init();
    qDebug() << "m_fd----------->" <<m_fd;
    for (const QString& p:paths) {
        int ret = inotify_add_watch(m_fd, p.toStdString().c_str(), IN_ALL_EVENTS);
        if (ret == -1) {
            return false;
        }
    }
    return true;
}

bool InotifyManager::RemoveWatch(const QString &path){
    int ret = inotify_rm_watch(m_fd, currentPath.key(path));
    if (ret){
        qDebug() << "remove path error";
        return false;
    }
//    qDebug() << "remove path: " << path;

    for (QMap<int, QString>::Iterator i = currentPath.begin(); i != currentPath.end();){
        if (i.value().length() > path.length()){
            if (i.value().mid(0, path.length()) == path){
                qDebug() << i.value();
                /*--------------------------------*/
                //在此调用删除索引
                IndexGenerator::getInstance()->deleteAllIndex(new QStringList(path));
                /*--------------------------------*/
                currentPath.erase(i++);
            }
            else{
                i++;
            }
        }
        else{
            i++;
        }
    }
    qDebug() << path;
    //这个貌似不用删，先mark一下
    //currentPath.remove(currentPath.key(path));
    return true;
}

void InotifyManager::run(){

    char * p;
    char buf[BUF_LEN] __attribute__((aligned(8)));

    ssize_t numRead;

    for (;;) { /* Read events forever */
        numRead = read(m_fd, buf, BUF_LEN);
        if (numRead == 0) {
            qDebug() << "read() from inotify fd returned 0!";
        }
        if (numRead == -1) {
            qDebug() << "read";
        }
        qDebug() << "Read " << numRead << " bytes from inotify fd";

        /* Process all of the events in buffer returned by read() */

        for (p = buf; p < buf + numRead;) {
            struct inotify_event * event = reinterpret_cast<inotify_event *>(p);
            if(event->name[0] != '.'){
//            if(true){
                //这个位运算不要在意，只是懒得把文件夹、文件和事件排列组合了,只是看一下事件的类型
                qDebug() << "Read Event: " << num2string[(event->mask & 0x0000ffff)] << currentPath[event->wd] << QString(event->name) << event->cookie << event->wd;
                //num2string[event->mask & 0x0000ffff]
                IndexGenerator::getInstance()->creatAllIndex(new QStringList(currentPath[event->wd] + event->name));

                /*--------------------------------*/

                //传创建或移动过来的文件路径
                if((event->mask & IN_CREATE) | (event->mask & IN_MOVED_TO)){
                    //添加监视要先序遍历，先添加top节点
                    if (event->mask & IN_ISDIR){
                        AddWatch(currentPath[event->wd] + '/' + event->name);
                        Traverse_BFS(currentPath[event->wd] + '/' + event->name, true);
                    }
                    else {
                        //IndexGenerator::getInstance()->creatAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
                    }
                }
                else if((event->mask & IN_DELETE) | (event->mask & IN_MOVED_FROM)){
                    if (event->mask & IN_ISDIR){
                        RemoveWatch(currentPath[event->wd] + '/' + event->name);
                    }
                    else {
                        //这里调用删除索引
                        IndexGenerator::getInstance()->deleteAllIndex(new QStringList(currentPath[event->wd] + '/' + event->name));
                    }
                }
                /*--------------------------------*/
            }
            p += sizeof(struct inotify_event) + event->len;
        }
    }
}


InotifyManager::InotifyManager()
{

    m_fd = inotify_init();
    qDebug() << "m_fd----------->" <<m_fd;
    num2string.insert(IN_ACCESS, "IN_ACCESS");
    num2string.insert(IN_MODIFY, "IN_MODIFY");
    num2string.insert(IN_ATTRIB, "IN_ATTRIB");
    num2string.insert(IN_CLOSE_WRITE, "IN_CLOSE_WRITE");
    num2string.insert(IN_CLOSE_NOWRITE, "IN_CLOSE_NOWRITE");
    num2string.insert(IN_CLOSE, "IN_CLOSE");
    num2string.insert(IN_OPEN, "IN_OPEN");
    num2string.insert(IN_MOVED_FROM, "IN_MOVED_FROM");
    num2string.insert(IN_MOVED_TO, "IN_MOVED_TO");
    num2string.insert(IN_MOVE, "IN_MOVE");
    num2string.insert(IN_CREATE, "IN_CREATE");
    num2string.insert(IN_DELETE, "IN_DELETE");
    num2string.insert(IN_DELETE_SELF, "IN_DELETE_SELF");
    num2string.insert(IN_MOVE_SELF, "IN_MOVE_SELF");
    num2string.insert(IN_UNMOUNT, "IN_UNMOUNT");
    num2string.insert(IN_Q_OVERFLOW, "IN_Q_OVERFLOW");
    num2string.insert(IN_IGNORED, "IN_IGNORED");
    return;

}
