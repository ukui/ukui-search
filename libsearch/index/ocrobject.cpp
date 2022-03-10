#include "ocrobject.h"

OcrObject *OcrObject::m_instance = nullptr;
once_flag g_instanceFlag;

OcrObject *OcrObject::getInstance()
{
    std::call_once(g_instanceFlag, [] () {
        m_instance = new OcrObject;
    });
    return m_instance;
}

void OcrObject::getTxtContent(QString &path, QString &textcontent)
{
    m_api = new tesseract::TessBaseAPI();
    if (m_api->Init(NULL, "chi_sim")) {
        qDebug() << "Could not initialize tesseract.\n";
        return;
    }
    m_api->SetVariable("user_defined_dpi", "1080");//图片中未标明分辨率的默认设置为1080

    Pix *image = pixRead(path.toStdString().data());
    if (!image) {
        qDebug() << "path:" << path <<" pixRead error!";
        if (m_api) {
            m_api->End();
            delete m_api;
            m_api = nullptr;
        }
        return;
    }
    m_api->SetImage(image);
    textcontent = m_api->GetUTF8Text();
    qDebug() << "path:" << path << " Text:" << textcontent;
    pixDestroy(&image);
    m_api->Clear();

    if (m_api) {
        m_api->End();
        delete m_api;
        m_api = nullptr;
    }

//多进程版本
//    tesseract::TessBaseAPI  *api = new tesseract::TessBaseAPI();
//    if (api->Init(NULL, "chi_sim")) {
//        qDebug() << "Could not initialize tesseract.\n";
//        return;
//    }
//    api->SetVariable("user_defined_dpi", "1080");//图片中未标明分辨率的默认设置为1080

//    Pix *image = pixRead(path.toStdString().data());
//    if (!image) {
//        qDebug() << "path:" << path <<" pixRead error!";
//        if (api) {
//            api->End();
//            delete api;
//            api = nullptr;
//        }
//        return;
//    }
//    api->SetImage(image);
//    textcontent = api->GetUTF8Text();
//    qDebug() << "path:" << path << " Text:" << textcontent;
//    pixDestroy(&image);
//    api->Clear();

//    if (api) {
//        api->End();
//        delete api;
//        api = nullptr;
//    }
}

OcrObject::OcrObject(QObject *parent) : QObject(parent)
{
    init();
}

OcrObject::~OcrObject()
{
    if (m_api) {
        m_api->End();
        delete m_api;
        m_api = nullptr;
    }
}

void OcrObject::init()
{
    m_api = new tesseract::TessBaseAPI();
    if (m_api->Init(NULL, "chi_sim")) {
        qDebug() << "Could not initialize tesseract.\n";
        return;
    }
    m_api->SetVariable("user_defined_dpi", "1080");//图片中未标明分辨率的默认设置为1080
}
