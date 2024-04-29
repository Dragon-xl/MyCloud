#include "common.h"
#include<QFile>
#include<QDebug>
#include<QJsonDocument>
#include<QJsonParseError>
#include<QJsonObject>
#include<QVariant>
#include"des.h"
#include<QCryptographicHash>
#include<QDir>
#include<QStringList>
#include<QTime>
#include<QRandomGenerator>
int Common::curFolderId=0;
QString Common::m_typePath = FILETYPEPATH;
QStringList Common::m_typeList=QStringList();
QNetworkAccessManager* Common::m_manager = new QNetworkAccessManager;
Common::Common(QObject *parent)
    : QObject{parent}
{
    //m_typePath = CONFFILE;
}
/**
 * @brief Common::getConfValue 读取想要的配置文件信息
 * @param path  配置文件路径
 * @param titleKey 配置文件的哪一部分 如login register等
 * @param key key
 * @return 失败返回"" 成功返回信息
 */
QString Common::getConfValue(QString titleKey, QString key,QString path)
{
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"Common::getConfValue file open failed"<<file.errorString();
        return "";
    }
    QByteArray content =  file.readAll();
    file.close();

    QJsonParseError err ;
    QJsonDocument doc =  QJsonDocument::fromJson(content,&err);
    if(err.error == QJsonParseError::NoError)
    {
        if(doc.isNull()||doc.isEmpty())
        {
           qDebug()<<"doc is Null || isEmpty";
            return "";
        }

        if(doc.isObject())
        {
           QJsonObject obj =  doc.object();
           QJsonObject obj1 =  obj.value(titleKey).toObject();
           return obj1.value(key).toString();
        }

    }
    else
    {
        qDebug()<<"error:"<<err.errorString();
    }
    return "";
}

void Common::writeWebInfo(QString ip, QString port, QString path)
{
    //读取出Login信息
    QString user = getConfValue("login","user");
    QString passwd = getConfValue("login","pwd");
    QString remember = getConfValue("login","remember");
    QMap<QString,QVariant> login;
    login.insert("user",user);
    login.insert("pwd",passwd);
    login.insert("remember",remember);

    QMap<QString,QVariant> type_Path;
    type_Path.insert("path",m_typePath);

    QMap<QString,QVariant> webServer;
    webServer.insert("ip",ip);
    webServer.insert("port",port);

    QMap<QString,QVariant> json;
    json.insert("login",login);
    json.insert("type_path",type_Path);
    json.insert("web_server",webServer);

    QJsonDocument doc =  QJsonDocument::fromVariant(json);
    if(doc.isNull()||doc.isNull())
    {
        qDebug()<<"writeWebInfo write info doc is null";
        return;
    }
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"writeWebInfo write info file open failed";
        return;
    }
    QByteArray wfile = doc.toJson();
    file.write(wfile);
    file.close();

}

void Common::writeLoginInfo(QString user, QString pwd, bool remember, QString path)
{
    //des加密
    unsigned char encUser[1024]={0};
    int encUserLen;
    unsigned char encPwd[1024]={0};
    int encPwdLen;
    int ret = DesEnc((unsigned char*)user.toLocal8Bit().data(),user.toLocal8Bit().length(),encUser,&encUserLen);
    if(ret != 0)
    {
        qDebug()<<"user enc failed";
        return;
    }
    ret = DesEnc((unsigned char*)pwd.toLocal8Bit().data(),pwd.toLocal8Bit().length(),encPwd,&encPwdLen);
    if(ret != 0)
    {
        qDebug()<<"pwd enc failed";
    }

    //使用base64进一步编码
    QMap<QString,QVariant> login;
    login.insert("user",QByteArray((char*)encUser,encUserLen).toBase64());
    login.insert("pwd",QByteArray((char*)encPwd,encPwdLen).toBase64());
    if(remember==true)
    {
        login.insert("remember","yes");
    }
    else
    {
        login.insert("remember","no");
    }

    QMap<QString,QVariant> type_path;
    type_path.insert("path",m_typePath);

    QMap<QString,QVariant> web_server;
    web_server.insert("ip",getConfValue("web_server","ip"));
    web_server.insert("port",getConfValue("web_server","port"));

    QMap<QString,QVariant> json;
    json.insert("login",login);
    json.insert("type_path",type_path);
    json.insert("web_server",web_server);

    QJsonDocument doc = QJsonDocument::fromVariant(json);
    if(doc.isNull()||doc.isEmpty())
    {
        qDebug()<<"writeLoginInfo doc failed";
        return;
    }
    QFile file(path);
    if(!file.open(QIODevice::WriteOnly))
    {
        qDebug()<<"writeLoginInfo open file failed";
        return;
    }
    ret = file.write(doc.toJson());
    if(ret == -1)
    {
        qDebug()<<"writeLoginInfo file.write failed";
        return;
    }
    file.close();
}
QString Common::getFileMd5(QString filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"getFileMd5 file.open failed";
        return "";
    }
    qint64 fileSize = file.size();
    QByteArray buf;
    QCryptographicHash hash(QCryptographicHash::Md5);
    while(1)
    {
        if(fileSize>0)
        {
          buf = file.read(qMin(fileSize,qint64(1024*4)));
          hash.addData(buf);
          fileSize-=1024*4;
          buf.resize(0);
        }
        else
        {
          break;
        }

    }
    return hash.result().toHex();


}
QString Common::getStrMd5(QString str)
{
   if(str == NULL)
   {
    return "";
   }
  QByteArray ha = QCryptographicHash::hash(str.toLocal8Bit(),QCryptographicHash::Md5);
   return ha.toHex();
}

void Common::getFileTypeList(QString typePath)
{
   QDir dir(typePath);
   if(!dir.exists())
   {
    dir.mkdir(typePath);
    qDebug()<<"创建目录:"<<typePath;
   }
   dir.setFilter(QDir::Files|QDir::NoDot|QDir::NoDotDot|QDir::NoSymLinks);
   dir.setSorting(QDir::Size|QDir::Reversed);
   QStringList list = dir.entryList();
   for(int i = 0 ;i<list.size();i++)
   {
    m_typeList.append(list.at(i));
   }
}

QString Common::getFileType(QString type)
{
   QString myType = type+".png";
   if(!m_typeList.contains(myType))
   {
    return m_typePath+"/"+"others.png";

   }
   return m_typePath + "/"+myType;
}

QNetworkAccessManager *Common::getManager()
{
   return m_manager;
}

QString Common::getCode(QByteArray json)
{
   QJsonParseError err;
   QJsonDocument doc = QJsonDocument::fromJson(json,&err);
   if(err.error == QJsonParseError::NoError)
   {
    if(doc.isNull()||doc.isEmpty())
    {

        qDebug()<<"getCode doc failed";
        return "";
    }
    if(!doc.isObject())
    {
        qDebug()<<"getCode not object";
        return "";
    }
    QJsonObject obj = doc.object();
    return obj.value("code").toString();

   }
   else
   {
    qDebug()<<"getCode QJsonParseError:"<<err.errorString();
    return"";
   }
}
/**
 * @brief Common::writeRecord 写用户操作日志
 * @param code  服务器返回的状态码
 * @param fileName 操作的文件名称
 * @param user  用户
 * @param path  record文件目录
 */
void Common::writeRecord(QString code,QString fileName,QString user,QString path)
{
   QString codeStr;
   QString file_path = path+user;
   QDir dir(path);
   QString content;
   if(!dir.exists())
   {
    if(!dir.mkpath(path))
    {
        qDebug()<<"writeRecord mkpath failed";
        return;
    }
   }
   QFile file(file_path);
   if(!file.exists())
   {
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text))
    {
        qDebug()<<"writeRecord file.open(QIODevice::WriteOnly failed";
        return;
    }

   }
   else
   {
    if(!file.open(QIODevice::Append|QIODevice::Text))
    {
        qDebug()<<"writeRecord file.open(QIODevice::Append|QIODevice::Text)";
        return;
    }
   }

   /*
               秒传文件：
                    文件已存在：{"code":"005"}
                    秒传成功：  {"code":"006"}
                    秒传失败：  {"code":"007"}
                上传文件：
                    成功：{"code":"008"}
                    失败：{"code":"009"}
                下载文件：
                    成功：{"code":"010"}
                    失败：{"code":"011"}
    */
   if(code == "005")
   {
    codeStr="上传失败，文件已存在";
   }
   if(code == "006")
   {
    codeStr="上传成功";
   }
   if(code == "007")
   {
    codeStr="上传失败";
   }
   if(code == "008")
   {
    codeStr="上传文件成功";
   }
   if(code == "009")
   {
    codeStr="上传文件失败";
   }
   if(code == "010")
   {
    codeStr="下载文件成功";
   }
   if(code == "011")
   {
    codeStr="下载文件失败";
   }

   //获取系统时间
   QString curTime =  QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
   content=QString("[%1]\t%2\t%3\r\n").arg(curTime).arg(fileName).arg(codeStr);

   QTextStream out(&file);
   out<<content<<Qt::endl;
   file.close();

}
/**
 * @brief Common::getBoundary 产生分割线
 * @return 分割线
 */
QString Common::getBoundary()
{

   QString tmp;
   QRandomGenerator *generator = QRandomGenerator::global();

   // 48~122, '0'~'A'~'z'
    for (int i = 0; i < 16; ++i)
   {
        int randomValue = generator->bounded(62); // 62 是因为我们有 10 个数字、26 个大写字母和 26 个小写字母
        if (randomValue < 10)
        {
            tmp.append(QChar('0' + randomValue)); // 数字 0-9
        }
        else if (randomValue < 36)
        {
            tmp.append(QChar('A' + randomValue - 10)); // 大写字母 A-Z
        }
        else
        {
                tmp.append(QChar('a' + randomValue - 36));
        }// 小写字母 a-z
    }


   return QString("------WebKitFormBoundary%1").arg(tmp);
}

















