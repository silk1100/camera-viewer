#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <iostream>
#include <QTimer>
#include <QLabel>
#include <QScopedPointer>
#include <QScopedArrayPointer>

#include <opencv2/core.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_rdbtn_jet_clicked();

    void on_rdbtn_gray_clicked();

    void on_rdbtn_smooth_clicked();

    void on_btn_CMON_clicked();

    void on_btn_SCST_clicked();

    void timeout_slot();

    void on_hrScrl_timer_valueChanged(int value);

    void on_rdbtn_inf_clicked();

    void on_rdbtn_vir_clicked();

private:
    Ui::MainWindow *ui;

    // Qt variables
    QScopedPointer<QTimer> timer;
    QScopedArrayPointer<QWidget> widget_array;
    QScopedArrayPointer<QLabel> label_array;

    // OpenCV variables
    void getAllCamId(std::vector<int> &camIds, int camnum);
    std::vector<int> camIds;
    int colorMap;
    int filter;
    int currTab;
    bool camON;
    std::vector<cv::VideoCapture> vid;

    enum MYCOLORMAP {
        JET, GRAY
    };
    enum MYFILTERS {
        SMOOTH
    };

};
#endif // MAINWINDOW_H
