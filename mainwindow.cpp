#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    getAllCamId(camIds, 3);
    for(auto i: camIds) cout<<"Camera detected at:" <<i<<endl;

    // Exit if you could not find any cameras
    if (!camIds.size()) return;

    // Initialize Pointers
    timer.reset(new QTimer());
    widget_array.reset(new QWidget[camIds.size()]);
    label_array.reset(new QLabel[camIds.size()]);

    // Initialize VideoCapture
//    int k=0;
//    for(int i=0; i<camIds.size(); ++i){
//        vid[i].open(i, cv::CAP_ANY);
//    }

    // Set signals and slots
    connect(timer.data(), &QTimer::timeout,
            this, &MainWindow::timeout_slot);


    timer->start(1000.0/5.0);
//    ui->centralwidget->repaint();
    ui->lbl_cam0->repaint();


    // Initialize GUI state
    currTab = 0;
    colorMap=0;
    filter=0;
    camON=false;
    ui->btn_CMON->setText("Camera OFF");
    ui->rdbtn_gray->click();
    colorMap = MYCOLORMAP::GRAY;
    styleTheGUI();
}

void MainWindow :: styleTheGUI(){
    QString styleWidget("QWidget {background-color:#BADBE6; font-family: serif;}");

    QString styleGroupBox("QGroupBox {background-color:#E8B600; font: bold serif 14px;}");
    QString stylePushBtn("QPushButton {background-color:#F1ECE6; height: 80px; font: bold serif 14px;}");
    QString styleRadioBtn("QRadioButton {background-color:#F1ECE6; font: bold serif 14px;}");


    ui->centralwidget->setStyleSheet(
        styleWidget+styleGroupBox+stylePushBtn+styleRadioBtn
    );

}

void MainWindow :: getAllCamId(vector<int> &camIds, int num){
    for (int i=0; i<1000; ++i){
        try{
            VideoCapture cap(i);
            Mat img;
            cap.read(img);
            if(!img.empty()){
                camIds.push_back(i);
                vid.push_back(cap);
                if(camIds.size()==num) break;
            }
        }catch(...){
            cout<<"An error was raised at "<<i<<endl;
        }
    }

    camDevList = QMediaDevices::videoInputs();
    for(auto c: camDevList) camDescriptions.push_back(c.description().toStdString());

}

MainWindow::~MainWindow()
{
    delete ui;
    for(int i=0; i<vid.size(); ++i)
    vid[i].release();
}

void MainWindow::on_rdbtn_jet_clicked()
{
    colorMap = MYCOLORMAP::JET;
}


void MainWindow::on_rdbtn_gray_clicked()
{
    colorMap = MYCOLORMAP::GRAY;
}


void MainWindow::on_btn_CMON_clicked()
{
    if(camON){
        camON = false;
        ui->btn_CMON->setText("camera OFF");
        for(int i=0; i<vid.size(); ++i)
        vid[i].release();
    }else{
       camON=true;
       ui->btn_CMON->setText("camera ON");
       for (int i=0; i<vid.size(); ++i)
       vid[i].open(currTab);
    }
}


void MainWindow::on_btn_SCST_clicked()
{
    QPixmap pim0 = ui->lbl_cam0->pixmap();
    QPixmap pim1 = ui->lbl_cam1->pixmap();
    QPixmap pim2 = ui->lbl_cam2->pixmap();
    QPixmap pim3 = ui->lbl_cam3->pixmap();
    pim0.save("image.png");
    QVector<QPixmap> pimVec = {pim0, pim1, pim2, pim3};
    QDir outputdir("output");
    if(!outputdir.exists()){
        QDir().mkdir("output");
    }
    QString uOutput =  outputdir.dirName()+QDir::separator()+QDateTime::currentDateTime().toString("yy_MM_dd");
    if(!QDir(uOutput).exists()){
        QDir().mkdir(uOutput);
    }
    cout<<uOutput.toStdString()<<endl;
    QString camName = QString::fromStdString(camDescriptions[0]);
    int i=0;
    for (auto sname: camDescriptions){
        QString ssname = QString::fromStdString(sname);
        ssname = ssname.replace(" ","_");
        QString outputfile = uOutput+QDir::separator()+
                          QDateTime::currentDateTime().toString("hh_mm_ss_")
                          +ssname+QString(".png");
        cout<<outputfile.toStdString()<<endl;
        pimVec[i++].save(outputfile);
    }


//    QDateTime::date()
//    pim0.save(QDateTime::currentDateTime())
}


void MainWindow::timeout_slot()
{
    if(!camON){
        ui->lbl_cam0->clear();
        ui->lbl_cam1->clear();
        ui->lbl_cam2->clear();
        ui->lbl_cam3->clear();
        return;
    }

    for(int i=0; i<vid.size(); ++i){
        Mat out;
        cout<<"Device "<<i<<": "<<camDescriptions[i]<<endl;
        vid[camIds[i]].open(camIds[i]);
        vid[camIds[i]].read(out);
        cout<<vid[camIds[i]].get(cv::CAP_PROP_FOURCC)<<endl;
        if(out.empty()) return;
        QImage *im;
        if(i){
            cout<<out.type()<<endl;
            cvtColor(out, out, COLOR_BGR2GRAY);
            if(colorMap == MYCOLORMAP::JET){
                applyColorMap(out, out, COLORMAP_JET);
            }
            else if(colorMap == MYCOLORMAP::INFERNO){
                applyColorMap(out, out, COLORMAP_INFERNO);
            }
            else if(colorMap == MYCOLORMAP::VIRIDIS){
                applyColorMap(out, out, COLORMAP_VIRIDIS);
            }
        }

        im=new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_BGR888);
        showOnLabel(i, im);
        delete im;
    }
}

void MainWindow :: showOnLabel(int camIdx, QImage *im){

    switch(camIdx){
    case 0:
    ui->lbl_cam0->setPixmap(QPixmap::fromImage(*im));
    break;
    case 1:
    ui->lbl_cam1->setPixmap(QPixmap::fromImage(*im));
    break;
    case 2:
    ui->lbl_cam2->setPixmap(QPixmap::fromImage(*im));
    break;
    case 3:
    ui->lbl_cam3->setPixmap(QPixmap::fromImage(*im));
    break;
    case 4:
    ui->lbl_cam0->setPixmap(QPixmap::fromImage(*im));
    break;
    default:
    break;
    }
}


void MainWindow::on_hrScrl_timer_valueChanged(int value)
{
    /*
     * 1 f/s ==> 1/1000 ms
     *
     * 2 f/s ==> 2/1000 ms
     * 5 f/s --> 5/00
     *
    */
    cout<<value<<endl;
    ui->lbl_frameRate->setText(QString("Frame rate: ")+QString::number(value));
    ui->lbl_frameRate->repaint();
    timer->stop();
    timer->setInterval(1000.0/(double)value);
    timer->start();
}


void MainWindow::on_rdbtn_inf_clicked()
{
    colorMap = MYCOLORMAP::INFERNO;
}


void MainWindow::on_rdbtn_vir_clicked()
{
    colorMap = MYCOLORMAP::VIRIDIS;
}

