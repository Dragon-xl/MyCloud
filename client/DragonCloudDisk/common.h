#ifndef COMMON_H
#define COMMON_H

#include "qlistwidget.h"
#include <QObject>
#include<QNetworkAccessManager>
#include<QByteArray>
#define CONFFILE       "conf/conf.json"
#define FILETYPEPATH   "conf/fileType"
#define RECORDFILE     "conf/record/"
enum TransferStatus{Download, Uplaod, Recod};
// 正则表达式
#define USER_REG        "^[0-9a-zA-Z][a-zA-Z0-9_@#$]{3,15}$"
#define PASSWD_REG      "^[a-zA-Z0-9_@#$]{6,18}$"
#define PHONE_REG       "^1[0-9]{10}$"
#define EMAIL_REG       "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$"
#define IP_REG          "((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)"
#define PORT_REG        "^[1-9]$|(^[1-9][0-9]$)|(^[1-9][0-9][0-9]$)|(^[1-9][0-9][0-9][0-9]$)|(^[1-6][0-5][0-5][0-3][0-5]$)"
struct FileInfo
{
    QString md5;            // 文件md5码
    QString filename;       // 文件名字
    QString user;           // 用户
    QString time;           // 上传时间
    QString url;            // url
    QString type;           // 文件类型
    qint64 size;            // 文件大小
    int shareStatus;        // 是否共享, 1共享， 0不共享
    int pv;                 // 下载量
    QListWidgetItem *item;  // list widget 的item
};
class Common : public QObject
{
    Q_OBJECT
public:
    explicit Common(QObject *parent = 0);
    QString getConfValue(QString titleKey,QString key,QString path = CONFFILE);
    void writeWebInfo(QString ip,QString port,QString path = CONFFILE);
    void writeLoginInfo(QString user,QString pwd,bool remember,QString path = CONFFILE);
    QString getFileMd5(QString filePath);
    QString getStrMd5(QString str);
    void getFileTypeList(QString typePath = FILETYPEPATH);
    QString getFileType(QString type);
    static QNetworkAccessManager* getManager();
    QString getCode(QByteArray json);
    void writeRecord(QString code,QString filename,QString user,QString path=RECORDFILE);
    QString getBoundary();




signals:
private:
    static QString m_typePath;
    static QStringList m_typeList;
    static QNetworkAccessManager* m_manager;


};

#endif // COMMON_H
