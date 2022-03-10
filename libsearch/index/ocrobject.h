#ifndef OCROBJECT_H
#define OCROBJECT_H

#include <QObject>
#include <mutex>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDebug>

using namespace std;
class OcrObject : public QObject
{
    Q_OBJECT
public:
    static OcrObject* getInstance();

    void getTxtContent(QString &path, QString &textcontent);

protected:
    explicit OcrObject(QObject *parent = nullptr);
    ~OcrObject();

private:
    static OcrObject *m_instance;

    tesseract::TessBaseAPI  *m_api = nullptr;
    void init();

    class Garbo
    {
    public:
        ~Garbo() {
            if (OcrObject::m_instance)
                delete OcrObject::m_instance;
        }
        static Garbo g_garbo;
    };

};

#endif // OCROBJECT_H
