#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

#include <iostream>

#include "ffmpeg_error.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actOpenFile_triggered()
{
    QString curPath = QDir::currentPath();
    QString title = "打开文件";
    QString filter = "所有文件(*.*)";
    m_infilename = QFileDialog::getOpenFileName(this, title, curPath, filter);
    qDebug() << m_infilename;
}



void MainWindow::on_actSaveFile_triggered()
{
    QString curPath = QCoreApplication::applicationDirPath();   // 保存文件该路径不可改变
    QString title = "保存文件";
    QString filter = "所有文件(*.*)";
    m_outfilename = QFileDialog::getSaveFileName(this, title, curPath, filter);
    qDebug() << m_outfilename;
}

void MainWindow::on_pushButton_clicked()
{
    int ret = m_ffmpegcodec.FFmpegCodec_Init(m_infilename.toLatin1().data());
    if(ret != FFMPEG_OK)
        return;

    m_ffmpegcodec.FFmpegCodec_SetSaveFile(m_outfilename.toLatin1().data());
    m_ffmpegcodec.FFmpegCodec_Process();
    std::cout << "finish" << std::endl;
}

