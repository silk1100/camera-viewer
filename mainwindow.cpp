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


void MainWindow::on_rdbtn_smooth_clicked()
{
    filter = MYFILTERS::SMOOTH;

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
       vid[i].open(currTab, cv::CAP_ANY);
    }
}


void MainWindow::on_btn_SCST_clicked()
{
    switch (currTab) {
    case 0:
        cout<<"MANTIS"<<endl;
        break;
    case 1:
        cout<<"LWIS"<<endl;
        break;
    case 2:
        cout<<"VIS"<<endl;
        break;
    case 3:
        cout<<"SWIR"<<endl;
        break;
    default:
        break;
    }
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

        vid[camIds[i]].open(camIds[i]);
        vid[camIds[i]].read(out);
        if(out.empty()) return;
        QImage *im;
        if(!i) im=new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_BGR888);
        else{
            if(colorMap == MYCOLORMAP::JET){
                cvtColor(out, out, COLOR_BGR2GRAY);
                applyColorMap(out, out, COLORMAP_JET);
                im=new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_BGR888);
            }
        }
//        if(out.channels()
//            if(!i)
//            im = new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_BGR888);
//            else{
//                cvtColor(out, out, COLOR_BGR2GRAY);
//            	im = new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_Grayscale8);
//                if

//            }
//        }else{
//        if(colorMap==MYCOLORMAP::JET){
//            applyColorMap(out, out, COLORMAP_JET);
//            im = new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_BGR888);

//        }else{

//            im = new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_Grayscale8);
//        }

//        }
//        cout<<"currTab="<<currTab<<endl;
        switch(i){
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
        delete im;
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

}


void MainWindow::on_rdbtn_vir_clicked()
{

}

