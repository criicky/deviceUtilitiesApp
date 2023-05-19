#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QNetworkSettingsManager>
#include <QJniObject>
#include <QtQml/QQmlEngine>
#include <QtQuickControls2/QQuickStyle>
#include <QQuickItem>
#include <QObject>
#include <QCoreApplication>
#include <QCoreApplication>
#include <QtCore/private/qandroidextras_p.h>
#include <QNetworkSettingsServiceModel>
#include <QNetworkSettingsService>
#include <QNetworkSettingsInterfaceModel>
#include <QNetworkSettingsInterface>

QJniEnvironment env;
QJniObject activity;
QJniObject context;
QJniObject connectivityManager;
QJniObject telephonyManager;
QJniObject wifiManager;
QList<QNetworkSettingsService *> serviceList;
QString currentSSID; //using it even when there is a cellular data connection, in that case ill use its imei(if i can get it)
QString ssid;

/********************************************************

I did not finish this because i thought i could merge this with the other project but had a problem while registering
native methods for some problem of cast

********************************************************/

bool checkExistence(QString ssid)
{
    foreach (QNetworkSettingsService *service, serviceList) {
        if(ssid == service->id())
        {
            return true;
        }
    }
    return false;
}

void removeService(QString ssid)
{
    int i=0;
    foreach (QNetworkSettingsService *service, serviceList) {
        if(ssid == service->id())
        {
            serviceList.removeAt(i);
        }
        i++;
    }
}

QString getSSID(QJniObject networkCapabilities)
{
    if(networkCapabilities.callMethod<jboolean>("hasTransport","(I)Z",0)) //cellular data
    {
        QJniObject packageManager = context.callObjectMethod("getPackageManager","()Landroid/content/pm/PackageManager;");

        bool hasFeature = packageManager.callMethod<jboolean>("hasSystemFeature","(Ljava/lang/String;)Z",
                                                              QJniObject::fromString("android.hardware.telephony.gsm").object<jstring>());

        ssid = telephonyManager.callObjectMethod("getSimCarrierIdName","()Ljava/lang/CharSequence;").toString();
    }
    else if(networkCapabilities.callMethod<jboolean>("hasTransport","(I)Z",1)) //wifi
    {
        QJniObject wifiInfo = wifiManager.callObjectMethod("getConnectionInfo",
                                                             "()Landroid/net/wifi/WifiInfo;");
        ssid = wifiInfo.callObjectMethod("getSSID","()Ljava/lang/String;").toString();
    }
    return ssid;
}

QNetworkSettingsService *getService(QString ssid)
{
    foreach (QNetworkSettingsService *service, serviceList)
    {
        if(ssid == service->id())
        {
            return service;
        }
    }
    return nullptr;
}

void capabilities(JNIEnv *env,jobject thiz, jobject wifiInfo, jobject network, jobject networkCapabilities)
{ //need to retreive the service from the list and update its capabilities
    QJniObject nw = (QJniObject) network;
    QJniObject activeNetwork = connectivityManager.callObjectMethod("getActiveNetwork","()Landroid/net/Network;");
    //qWarning() << "capabilities active network: " << activeNetwork.toString() << " network: " << nw.toString();

    //need to check the transport to know which object i should use
    QJniObject capabilities = (QJniObject) networkCapabilities;
    ssid = getSSID(capabilities);
    QNetworkSettingsService *service = getService(ssid);
}

void properties(JNIEnv *env,jobject thiz,jobject network,jobject linkProperties)
{ //here i need to find the service in the list and then update the properties inside using the singleton class
    QJniObject nw = (QJniObject) network;
    QJniObject activeNetwork = connectivityManager.callObjectMethod("getActiveNetwork","()Landroid/net/Network;");
    //qWarning() << "properties active network: " << activeNetwork.toString() << " network: " << nw.toString();
}

void available(JNIEnv *env,jobject thiz,jobject network)
{ //here i need to check if the network is already in my list
    QJniObject nw = (QJniObject) network;
    //QJniObject activeNetwork = connectivityManager.callObjectMethod("getActiveNetwork","()Landroid/net/Network;");
    //qWarning() << "available active network: " << activeNetwork.toString() << " network: " << nw.toString();
    QJniObject networkCapabilities = connectivityManager.callObjectMethod("getNetworkCapabilities",
                                                                          "(Landroid/net/Network;)Landroid/net/NetworkCapabilities;",
                                                                          nw.object<jobject>());
    ssid = getSSID(networkCapabilities);
    qDebug() << "available getSSID: " << ssid;
    if(!checkExistence(ssid))
    {
        QNetworkSettingsService *service = new QNetworkSettingsService(ssid);
        serviceList.append(service);
    }

    //everytime i lose a connection or theres a new connection available i need to check if the activeNetwork is changed
    //in that case i need to update the lists and the objects that point to the current connection and send a signal
    //if it did not change then i need to update the lists and send a signal just to notify that change
    //i dont know if its possible that the id already exists in the list because everytime i lose a connection i clear
    //that service, the only case is when two connections have the same id (so the ssid or the sim name)
}
void lost(JNIEnv *env,jobject thiz,jobject network)
{
    QJniObject nw = (QJniObject) network;
    QJniObject activeNetwork = connectivityManager.callObjectMethod("getActiveNetwork","()Landroid/net/Network;");
    //qWarning() << "lost active network: " << activeNetwork.toString() << " network: " << nw.toString();
    QJniObject networkCapabilities = connectivityManager.callObjectMethod("getNetworkCapabilities",
                                                                          "(Landroid/net/Network;)Landroid/net/NetworkCapabilities;",
                                                                          nw.object<jobject>());
    ssid = getSSID(networkCapabilities);
    qDebug() << "lost getSSID: " << ssid;
    if(checkExistence(ssid))
    {
        removeService(ssid);
    }
    //similar to available i neet to check if the saved objects contains the connection that got lost and clear them
    //after that i need to remove the service
    //m_currentssid and m_currentWifi/Wiredconnection need to be cleared
    //i need then to check if there is an activeNetwork because i dont get a new signal from the device if there was
    //already an online connection. in that case i could have already the properties and capabilities so the user will
    //see them without having to wait
}

void checkActiveNetwork(QJniObject network)
{

}

int main(int argc, char *argv[])
{

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/untitled/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url); //apre la pagina data dall url che è contenuta nel progetto

    //QNetworkSettingsManager *m = new QNetworkSettingsManager();
    /* Example of how to use the library for android, there are other signals that the user can receive
    QObject::connect(m, &QNetworkSettingsManager::interfacesChanged,[&m](){
        QList<QNetworkSettingsInterface *> interfaces = m->interfaces()->getModel();
        if(!interfaces.isEmpty())
        {
            foreach(QNetworkSettingsInterface *item,interfaces)
            {
                qDebug() << item->name() << " " << item->state();
            }
        }
    });

    QObject::connect(m, &QNetworkSettingsManager::servicesChanged,[&m](){
        QList<QNetworkSettingsService*> services = qobject_cast<QNetworkSettingsServiceModel*>(m->services()->sourceModel())->getModel();
        if(!services.isEmpty())
        {
            for (const auto &service : services)
            {
                qDebug() << "Servizio" << service->id();
            }
        }
        else
        {
            qDebug() << "No active services";
        }
    });

    QObject::connect(m, &QNetworkSettingsManager::currentWifiConnectionChanged,[&m](){
        QNetworkSettingsService *service = qobject_cast<QNetworkSettingsService*>(m->currentWifiConnection());
        if(service != nullptr)
        {
            qDebug() << "Nome servizio:" << service->name()
                     << "ipv4: " << service->ipv4()->address()
                     << "ipv6: " << service->ipv6()->address();
        }
        else
        {
            qDebug() << "No active connection";
        }
    });*/

    activity = QNativeInterface::QAndroidApplication::context();
    context = activity.callObjectMethod("getApplicationContext",
                                                   "()Landroid/content/Context;");
    connectivityManager = context.callObjectMethod("getSystemService","(Ljava/lang/String;)Ljava/lang/Object;",
                                                   QJniObject::fromString("connectivity").object<jstring>());
    telephonyManager = context.callObjectMethod("getSystemService","(Ljava/lang/String;)Ljava/lang/Object;",
                                                   QJniObject::fromString("phone").object<jstring>());
    wifiManager = context.callObjectMethod("getSystemService","(Ljava/lang/String;)Ljava/lang/Object;",
                                           QJniObject::fromString("wifi").object<jstring>());

    QtAndroidPrivate::checkPermission("android.permission.READ_PRIVILEGED_PHONE_STATE")
        .then([](QtAndroidPrivate::PermissionResult result)
              {
                  if(result != QtAndroidPrivate::PermissionResult::Authorized)
                  {
                      //qDebug() << "autorizzato";
                  }
                  else
                  {
                      //qDebug() << "non autorizzato";
                  }
              });

    QJniObject activeNetwork = connectivityManager.callObjectMethod("getActiveNetwork","()Landroid/net/Network;");
    qDebug() << "active network: " << activeNetwork.toString();
    jclass receiver = env.findClass("receiver/mainReceiver");

    const JNINativeMethod method[] = {{"capabilities","(Landroid/net/wifi/WifiInfo;Landroid/net/Network;Landroid/net/NetworkCapabilities;)V",
                                       reinterpret_cast<void *>(capabilities)},
                                      {"available","(Landroid/net/Network;)V",
                                       reinterpret_cast<void *>(available)},
                                      {"lost","(Landroid/net/Network;)V",
                                       reinterpret_cast<void *>(lost)},
                                      {"properties","(Landroid/net/Network;Landroid/net/LinkProperties;)V",
                                       reinterpret_cast<void *>(properties)}};

    env.registerNativeMethods("receiver/brdReceiver",method,4);

    QJniObject intent = QJniObject::callStaticObjectMethod(receiver,"start",
                                                           "(Landroid/content/Context;)Landroid/content/Intent;",
                                                           context.object<jobject>());

    context.callMethod<void>("startActivity","(Landroid/content/Intent;)V",intent.object<jobject>());

    return app.exec(); //manda in loop l app finchè non viene chiusa in qualche modo
}
