#include "mainwindow.h"
#include <QtCharts>
QT_CHARTS_USE_NAMESPACE
#include "ui_mainwindow.h"
#include "stdio.h"
#include "QDebug"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mqtt_client = new QMQTT::Client;
    m_strProductKey = "hg74uqD7rrr";
    m_strDeviceName = "jiedian";
    m_strDeviceSecret = "ff389a7fcb5adace3b8eea3c230ba36d";
    m_strRegionId = "cn-shanghai";
    m_strHostName = m_strProductKey + ".iot-as-mqtt." + m_strRegionId + ".aliyuncs.com";
    m_strPubTopic = "/sys/hg74uqD7rrr/jiedian/thing/event/property/post";
    m_strSubTopic = "/sys/hg74uqD7rrr/jiedian/thing/event/property/post";
    mqtt_client->setHostName(m_strHostName);//设置域名
    mqtt_client->setPort(1883);//设置端口
    mqtt_client->setUsername(m_strDeviceName + "&" + m_strProductKey);
    mqtt_client->setClientId("hg74uqD7rrr.jiedian|securemode=2,signmethod=hmacsha256,timestamp=1684473235417|");

    mqtt_client->setPassword("112d95f00c4b31f487a0306063cd035ae26f540ff26cfec05f70e49f908d1364");
    mqtt_client->setKeepAlive(30);//保活心跳时间取值范围为30秒~1200秒。如果心跳时间不在此区间内，阿里云物联网平台会拒绝连接

    connect(mqtt_client,SIGNAL(connected()),this,SLOT(doConnected()));//连接成功


    QTimer *timer = new QTimer(this);
    timer->start(4200);
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    timer1 = new QTimer(this);
    timer1->start(3200);
    connect(timer1, SIGNAL(timeout()), this, SLOT(onTimeout1()));

    timer2 = new QTimer(this);
    timer2->start(3200);
    connect(timer2, SIGNAL(timeout()), this, SLOT(onTimeout2()));

    timer3 = new QTimer(this);
    timer3->start(3200);
    connect(timer3, SIGNAL(timeout()), this, SLOT(onTimeout3()));

    series = new QSplineSeries;
    series->setName("温度");

    chart = ui->widget->chart();
    chart->setTitle("节点1");

    chart->addSeries(series);
    chart->createDefaultAxes();
    num = 0;

    series_0 = new QSplineSeries;
    series_0->setName("湿度");

    chart->addSeries(series_0);
    chart->createDefaultAxes();
    num_0 = 0;

    series1 = new QSplineSeries;
    series1->setName("光强");

    chart1 = ui->widget_2->chart();
    chart1->setTitle("节点2");

    chart1->addSeries(series1);
    chart1->createDefaultAxes();
    num1 = 0;
    msg_tem=0;
    msg_humi=100;

    series2 = new QSplineSeries;
    series2->setName("浓度");

    chart2 = ui->widget_3->chart();
    chart2->setTitle("节点3");

    chart2->addSeries(series2);
    chart2->createDefaultAxes();
    num2 = 0;

    ui->frame->setAutoFillBackground(true);
    QColor c;
    c.setGreen(0);
    c.setRed(255);
    c.setBlue(0);
    ui->frame->setPalette(QPalette(c));

    ui->frame_2->setAutoFillBackground(true);
    QColor c1;
    c1.setGreen(0);
    c1.setRed(255);
    c1.setBlue(0);
    ui->frame_2->setPalette(QPalette(c1));

    ui->frame_3->setAutoFillBackground(true);
    QColor c2;
    c2.setGreen(0);
    c2.setRed(255);
    c2.setBlue(0);
    ui->frame_3->setPalette(QPalette(c2));


    //保存为TXT文件
    bool exist;
    QDir *folder = new QDir;
    exist = folder->exists("/home/lzc/loongson");//查看目录是否存在（例子是保存到桌面）
    if(!exist){//不存在就创建
        bool ok = folder->mkdir("/home/lzc/loongson");
         if(ok){
                QMessageBox::warning(this,tr("创建目录"),tr("创建成功!"));//添加提示方便查看是否成功创建
            }else{
                QMessageBox::warning(this,tr("创建目录"),tr("创建失败"));
            }
        }
        fileName1 = tr("/home/lzc/loongson/%1.txt").arg("temp");
        fileName2 = tr("/home/lzc/loongson/%1.txt").arg("light");
        fileName3 = tr("/home/lzc/loongson/%1.txt").arg("gas");
        dateTime = QDateTime::currentDateTime();
        qDebug() << dateTime.toString("yyyy-MM-dd hh:mm:ss ddd");

		
             // 钉钉机器人的Webhook地址
    webhookUrl = "https://oapi.dingtalk.com/robot/send?access_token=66c7d29ba50525790d0b6614ed04f4f58f09342a484f2b81b1bb63c71a55994f";
    i_count=0;
    i1=0;
    i2=0;
    i3=0;
    msg_disv1=0;
    msg_disv2=0;
    msg_disv3=0;


}

MainWindow::~MainWindow()
{
    delete ui;
     //f.close();
}
void MainWindow::doConnected()
{
    qDebug("connect succeed");


}

void MainWindow::on_pushButton_2_clicked()//打开TCP服务器
{

    server =new QTcpServer;
    server1 =new QTcpServer;
    server2 =new QTcpServer;

    server->listen(QHostAddress::Any,55559);
    server1->listen(QHostAddress::Any,55555);
    server2->listen(QHostAddress::Any,55557);

    connect(server,SIGNAL(newConnection()),this,SLOT(new_client()));
    connect(server1,SIGNAL(newConnection()),this,SLOT(new_client1()));
    connect(server2,SIGNAL(newConnection()),this,SLOT(new_client2()));
    ui->textBrowser->append("服务器已打开");
}

void MainWindow::new_client()
{

    ui->textBrowser->append("新客户端连接");
    //基于连接创建套接字
    socket=server->nextPendingConnection();
    //关联信号与槽
    connect(socket,SIGNAL(readyRead()),this,SLOT(read_data()));
    ui->frame->setAutoFillBackground(true);


}
void MainWindow::new_client1()
{

    ui->textBrowser->append("新客户1端连接");
    //基于连接创建套接字
    socket1=server1->nextPendingConnection();

    connect(socket1,SIGNAL(readyRead()),this,SLOT(read_data1()));
}
void MainWindow::new_client2()
{

    ui->textBrowser->append("新客户2端连接");
    //基于连接创建套接字
    socket2=server2->nextPendingConnection();

    connect(socket2,SIGNAL(readyRead()),this,SLOT(read_data2()));
}
void MainWindow::read_data()
{
    bool ok;

    ui->frame->setAutoFillBackground(true);
    QColor c;
    c.setGreen(255);
    c.setRed(0);
    c.setBlue(0);
    ui->frame->setPalette(QPalette(c));

   timer1->stop();

     msgg0=socket->readAll();
     msg_int=msgg0.toInt(&ok,10);


     ++i1;
     if(i1%3!=0)
     {
     if(i1%2==0)
     msg_tem=msg_int;
     else
     msg_humi=msg_int;
     }
     if(i1%3==0)
     {
      msg_v1= msg_int;
      i1=0;
     }


     if(msg_v1>32351)
        msg_disv1=100;
     if((msg_v1>30740)&&(msg_v1<32352))
         msg_disv1=75;

     if((msg_v1>26630)&&(msg_v1<26793))
         msg_disv1=50;
     if((msg_v1>10)&&(msg_v1<26793))
          {
         msg_disv1=25;

         QNetworkAccessManager *networkManager = new QNetworkAccessManager();
         QNetworkRequest request(webhookUrl);
         request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QString message = "节点1电池过低1!";
        QString postData = QString("{\"msgtype\":\"text\", \"text\":{\"content\":\"%1\"}}").arg(message);

        QNetworkReply *reply = networkManager->post(request, postData.toUtf8());
        QObject::connect(reply, &QNetworkReply::finished, [=]() {

            if (reply->error() == QNetworkReply::NoError) {

                qDebug() << "Message sent to DingTalk successfully!";
            } else {

                qDebug() << "Failed to send message to DingTalk. Error:" << reply->errorString();
            }


        });
     }
     if(msg_humi>80)
     {

         QNetworkAccessManager *networkManager = new QNetworkAccessManager();
         QNetworkRequest request(webhookUrl);
         request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QString message = "温湿度异常过高1!";
        QString postData = QString("{\"msgtype\":\"text\", \"text\":{\"content\":\"%1\"}}").arg(message);

        QNetworkReply *reply = networkManager->post(request, postData.toUtf8());
        QObject::connect(reply, &QNetworkReply::finished, [=]() {

            if (reply->error() == QNetworkReply::NoError) {

                qDebug() << "Message sent to DingTalk successfully!";
            } else {

                qDebug() << "Failed to send message to DingTalk. Error:" << reply->errorString();
            }

        });



     }
   ui->label_9->setText(QString::number(msg_tem));
    ui->label_10->setText(QString::number(msg_humi));
   ui->label_23->setText(QString::number(msg_disv1));

   QFile f(fileName1);
   if(!f.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){//追加写入 添加结束符\r\n
        QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
       return ;
   }else{
       f.write("temp_humi: "+QString::number(msg_tem).toUtf8()+" "+QString::number(msg_humi).toUtf8()+" "+dateTime.toString("yyyy-MM-dd hh:mm:ss ddd").toUtf8()+"\n");
   }

    ui->textBrowser->append(msgg0);

if((QString::number(msg_tem)<101)&&(QString::number(msg_humi)<101))
{
    QPointF p(num++,QString::number(msg_tem).toFloat());
    QPointF p_0(num++,QString::number(msg_humi).toFloat());

    //把点加到数组中
    list.append(p);
    series->replace(list);
    list_0.append(p_0);
    series_0->replace(list_0);
    //把线重新加到图表中


    chart->removeSeries(series);
    chart->removeSeries(series_0);
    chart->addSeries(series);
    chart->addSeries(series_0);

    chart->createDefaultAxes();
    ui->widget->setChart(chart);
}
}
void MainWindow::read_data1()
{
    bool ok;

     msgg1=socket1->readAll();
     msg_int1=msgg1.toInt(&ok,10);
     //msg_float1=msgg1.toFloat();
     qDebug()<<msgg1;
     qDebug()<<msg_int1;

     if(msg_v2>10400)
        msg_disv2=100;
     if((msg_v2>10376)&&(msg_v2<10400))
         msg_disv2=75;

     if((msg_v2>9853)&&(msg_v2<9869))
         msg_disv2=50;
     if((msg_v2>10)&&(msg_v2<9772))
       {

         msg_disv2=25;

         QNetworkAccessManager *networkManager = new QNetworkAccessManager();
         QNetworkRequest request(webhookUrl);
         request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QString message = "节点2电池过低1!";
        QString postData = QString("{\"msgtype\":\"text\", \"text\":{\"content\":\"%1\"}}").arg(message);

        QNetworkReply *reply = networkManager->post(request, postData.toUtf8());
        QObject::connect(reply, &QNetworkReply::finished, [=]() {

            if (reply->error() == QNetworkReply::NoError) {

                qDebug() << "Message sent to DingTalk successfully!";
            } else {

                qDebug() << "Failed to send message to DingTalk. Error:" << reply->errorString();
            }


        });

     }


     if(msg_light>1200)
     {
         QNetworkAccessManager *networkManager = new QNetworkAccessManager();
         QNetworkRequest request(webhookUrl);
         request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QString message = "光照异常过强1!";
        QString postData = QString("{\"msgtype\":\"text\", \"text\":{\"content\":\"%1\"}}").arg(message);

        QNetworkReply *reply = networkManager->post(request, postData.toUtf8());
        QObject::connect(reply, &QNetworkReply::finished, [=]() {

            if (reply->error() == QNetworkReply::NoError) {

                qDebug() << "Message sent to DingTalk successfully!";
            } else {

                qDebug() << "Failed to send message to DingTalk. Error:" << reply->errorString();
            }


        });


     }
     if((++i2)%2==0)
         msg_v2=msg_int1;
     else
         msg_light=msg_int1-1900;

    ui->label_12->setText(QString::number(msg_light));
    ui->label_24->setText(QString::number(msg_disv2));
    ui->textBrowser->append(msgg1);


    QFile f(fileName2);
    if(!f.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){//追加写入 添加结束符\r\n
         QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }else{
        f.write("light:"+msgg1+" "+dateTime.toString("yyyy-MM-dd hh:mm:ss ddd").toUtf8()+"\n");
    }

    ui->frame_2->setAutoFillBackground(true);
    QColor c1;
    c1.setGreen(255);
    c1.setRed(0);
    c1.setBlue(0);
    ui->frame_2->setPalette(QPalette(c1));
    timer2->stop();

    QPointF p1(num1++,QString::number(msg_light).toFloat());
    //把点加到数组中
    list1.append(p1);
    series1->replace(list1);

    //把线重新加到图表中
    chart1->removeSeries(series1);
    chart1->addSeries(series1);
    //设置窗口名字
   // this->setWindowTitle("DATA");
    //在图表中添加设置空坐标轴
    chart1->createDefaultAxes();
    ui->widget_2->setChart(chart1);
}
void MainWindow::read_data2()
{
    bool ok;


     msgg2=socket2->readAll();
     msg_int2=msgg2.toInt(&ok,10);
     if((++i3)%2==0)
        msg_v3=msg_int2;
     else
         msg_mq5=msg_int2;


     if(msg_v3>10448)
        msg_disv3=100;
     if((msg_v3>10215)&&(msg_v3<10448))
         msg_disv3=75;

     if((msg_v3>10140)&&(msg_v3<10167))
         msg_disv3=50;
     if((msg_v3>10)&&(msg_v3<9999))
        {
         msg_disv3=25;
         QNetworkAccessManager *networkManager = new QNetworkAccessManager();
         QNetworkRequest request(webhookUrl);
         request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        QString message = "节点3电池过低1!";
        QString postData = QString("{\"msgtype\":\"text\", \"text\":{\"content\":\"%1\"}}").arg(message);
        QNetworkReply *reply = networkManager->post(request, postData.toUtf8());
        QObject::connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error() == QNetworkReply::NoError) {
                qDebug() << "Message sent to DingTalk successfully!";
            } else {
                qDebug() << "Failed to send message to DingTalk. Error:" << reply->errorString();
            }

        });
     }
if(msg_mq5>1600)
{
    QNetworkAccessManager *networkManager = new QNetworkAccessManager();
    QNetworkRequest request(webhookUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QString message = "监测到有毒气体1!";
    QString postData = QString("{\"msgtype\":\"text\", \"text\":{\"content\":\"%1\"}}").arg(message);
    QNetworkReply *reply = networkManager->post(request, postData.toUtf8());
    QObject::connect(reply, &QNetworkReply::finished, [=]() {
       if (reply->error() == QNetworkReply::NoError) {
           qDebug() << "Message sent to DingTalk successfully!";
       } else {
           qDebug() << "Failed to send message to DingTalk. Error:" << reply->errorString();
       }
   });
}
     ui->label_25->setText(QString::number(msg_disv3));
    ui->label_11->setText(QString::number(msg_mq5));
    ui->textBrowser->append(msgg2);

    QFile f(fileName3);
    if(!f.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text)){//追加写入 添加结束符\r\n
         QMessageBox::warning(this,tr("错误"),tr("打开文件失败,数据保存失败"));
        return ;
    }else{
        f.write("gas:"+msgg2+" "+dateTime.toString("yyyy-MM-dd hh:mm:ss ddd").toUtf8()+"\n");
    }

    if(msg_mq5>2500)
    {


     QNetworkAccessManager *networkManager = new QNetworkAccessManager();
     QNetworkRequest request(webhookUrl);
     request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QString message = "1mqqq!";
    QString postData = QString("{\"msgtype\":\"text\", \"text\":{\"content\":\"%1\"}}").arg(message);

    QNetworkReply *reply = networkManager->post(request, postData.toUtf8());
    QObject::connect(reply, &QNetworkReply::finished, [=]() {

        if (reply->error() == QNetworkReply::NoError) {

            qDebug() << "Message sent to DingTalk successfully!";
        } else {

            qDebug() << "Failed to send message to DingTalk. Error:" << reply->errorString();
        }


    });
    }



    ui->frame_3->setAutoFillBackground(true);
    QColor c2;
    c2.setGreen(255);
    c2.setRed(0);
    c2.setBlue(0);
    ui->frame_3->setPalette(QPalette(c2));
    timer3->stop();

    QPointF p2(num2++,QString::number(msg_mq5).toFloat());
    //把点加到数组中
    list2.append(p2);
    series2->replace(list2);

    //把线重新加到图表中
    chart2->removeSeries(series2);
    chart2->addSeries(series2);
    //设置窗口名字

    //在图表中添加设置空坐标轴
    chart2->createDefaultAxes();
    ui->widget_3->setChart(chart2);
}
void  MainWindow::onTimeout()
{
    static uint64_t i1=0;
    i1++;

    //发布消息
    message = "{\"method\":\"thing.service.event.post\",\"id\":\"1075874688\",\"params\":{\"Speed\":22,\"Start\":1,\"Direction\":2},\"version\":\"1.0.0\"}";
    //QString mw="{\"method\":\"thing.service.event.post\",\"id\":\"1683440632851\",\"params\":{\"temperature\":22,\"Humidity\":1,\"Direction\":2},\"version\":\"1.0.0\"}";

    message1= "{\"id\":1683440632851,\"params\":{\"temperature\":12,\"Humidity\":12},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}";
    //sprintf(buff1,"{\"id\":1683440632851,\"params\":{\"temperature\":%d,\"Humidity\":%d},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}",msg,msg);
    //QString sql = QString("insert into stu2(id,name,calss) values(%1, '%2', %3);").arg(id).arg(name).arg(classno);
    QString sql = QString("{\"id\":1683440632851,\"params\":{\"temperature\":%1,\"Humidity\":%2},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}").arg(msg_tem).arg(msg_humi);
    QString sql1=QString("{\"id\":1684850962271,\"params\":{\"LightLux\":%1},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}").arg(msg_light);
    QString sql2=QString("{\"id\":1684850962271,\"params\":{\"GasConcentration\":%1},\"version\":\"1.0\",\"method\":\"thing.event.property.post\"}").arg(msg_mq5);

    QMQTT::Message send_msg(136,m_strPubTopic,sql.toLocal8Bit(),0);
    QMQTT::Message send_msg1(136,m_strPubTopic,sql1.toLocal8Bit(),0);
    QMQTT::Message send_msg2(136,m_strPubTopic,sql2.toLocal8Bit(),0);
    mqtt_client->publish(send_msg);
    mqtt_client->publish(send_msg1);
    mqtt_client->publish(send_msg2);
    timer1->start(3200);
    connect(timer1, SIGNAL(timeout()), this, SLOT(onTimeout1()));
    timer2->start(3200);
    connect(timer2, SIGNAL(timeout()), this, SLOT(onTimeout2()));
    timer3->start(3200);
    connect(timer3, SIGNAL(timeout()), this, SLOT(onTimeout3()));

}
void  MainWindow::onTimeout1()
{
    ui->frame->setAutoFillBackground(true);
    QColor c;
    c.setGreen(0);
    c.setRed(255);
    c.setBlue(0);
    ui->frame->setPalette(QPalette(c));

    i1=0;


}
void  MainWindow::onTimeout2()
{
    ui->frame_2->setAutoFillBackground(true);
    QColor c1;
    c1.setGreen(0);
    c1.setRed(255);
    c1.setBlue(0);
    ui->frame_2->setPalette(QPalette(c1));

i2=0;
}
void  MainWindow::onTimeout3()
{
    ui->frame_3->setAutoFillBackground(true);
    QColor c2;
    c2.setGreen(0);
    c2.setRed(255);
    c2.setBlue(0);
    ui->frame_3->setPalette(QPalette(c2));
i3=0;

}

void MainWindow::on_pushButton_clicked()
{
    //连接mqtt服务器
    mqtt_client->connectToHost();
    ui->textBrowser->append("已连接阿里云");
    //断开连接
    //mqtt_client->disconnectFromHost();
}

void MainWindow::on_pushButton_3_clicked()//导出数据
{
 QMessageBox::warning(this,tr("数据"),tr("导出数据成功"));

}
