#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    vector<int> camIds = getAllCamId();

    // Exit if you could not find any cameras
    if (!camIds.size()) return;

    // Initialize Pointers
    timer.reset(new QTimer());
    widget_array.reset(new QWidget[camIds.size()]);
    label_array.reset(new QLabel[camIds.size()]);

    int i=0;
    for(auto idx: camIds){
        cout<<idx<<endl;
        label_array[i].setParent(&widget_array[i]);
        label_array[i].setText(QString::number(idx));
        ui->tabWidget->addTab(&widget_array[i++], QString("Camera: ")+QString::number(idx));
    }

    // Set signals and slots
    connect(timer.data(), &QTimer::timeout,
            this, &MainWindow::timeout_slot);

    timer->start(1000.0/5.0);
    ui->centralwidget->repaint();


    // Initialize GUI state
    currTab = 0;
    colorMap=0;
    filter=0;
    camON=false;
    ui->btn_CMON->setText("Camera OFF");
}


std::vector<int> MainWindow :: getAllCamId(void){
    vector<int> output;
    for (int i=0; i<1000; ++i){
        VideoCapture cap(i);
        Mat img;
        cap.read(img);
        if(!img.empty()){
            output.push_back(i);
        }
    }

    return output;
}

MainWindow::~MainWindow()
{
    delete ui;
    vid.release();
}

void MainWindow::on_rdbtn_jet_clicked()
{
    colorMap = COLORMAP::JET;
}


void MainWindow::on_rdbtn_gray_clicked()
{
    colorMap = COLORMAP::GRAY;
}


void MainWindow::on_rdbtn_smooth_clicked()
{
    filter = FILTERS::SMOOTH;

}


void MainWindow::on_btn_CMON_clicked()
{
    if(camON){
        camON = false;
        ui->btn_CMON->setText("camera OFF");
        vid.release();
    }else{
       camON=true;
       ui->btn_CMON->setText("camera ON");
       vid.open(currTab, cv::CAP_V4L2);
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


void MainWindow::on_tabWidget_currentChanged(int index)
{
    currTab = index;
    if (currTab==0)
        vid.open(0, cv::CAP_V4L2);
    else
        vid.open(0, cv::CAP_V4L2);

}

void MainWindow::timeout_slot()
{
    if(!camON){
        ui->lbl_LWIR->clear();
        ui->lbl_MANTIS->clear();
        ui->lbl_VIS->clear();
        ui->lbl_VIS->clear();
        return;
    }

//    if(currTab) currTab=200; else currTab=0;

    Mat out;
    vid.read(out);
    if(out.empty()) return;
    QImage *im;
    if(out.channels() == 3){
        im = new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_BGR888);
    }else{
        im = new QImage(out.data, out.cols, out.rows, out.step, QImage::Format_Grayscale8);
    }
    cout<<"currTab="<<currTab<<endl;
    switch(currTab){
    case 0:
        ui->lbl_MANTIS->setPixmap(QPixmap::fromImage(*im));
        break;
    case 1:
        ui->lbl_LWIR->setPixmap(QPixmap::fromImage(*im));
        break;
    case 2:
        ui->lbl_VIS->setPixmap(QPixmap::fromImage(*im));
        break;
    case 3:
        ui->lbl_SWIR->setPixmap(QPixmap::fromImage(*im));
        break;
    case 4:
        ui->lbl_MANTIS->setPixmap(QPixmap::fromImage(*im));
        break;
    default:
        break;
    }
    delete im;
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

