#include "sdldisplay.h"
#include "ui_sdldisplay.h"

// Qt头文件
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QTest>


SdlDisplay::SdlDisplay(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SdlDisplay)
{
    ui->setupUi(this);
    m_statusLabel = new QLabel;

    ui->actPlay->setEnabled(false);
    ui->actSave->setEnabled(false);

    connect(&m_timer, SIGNAL(emit_decode()), &m_ffmpegCodec, SLOT(on_emitDecode()));
    connect(&m_ffmpegCodec, SIGNAL(emit_display()), &m_sdl, SLOT(on_emitShow()));
    connect(&m_ffmpegCodec, SIGNAL(emit_decode_finish()), &m_sdl, SLOT(on_emitDecodeFinish()));
    connect(&m_sdl, SIGNAL(emit_displayFinish()), this, SLOT(on_emitDisplayFinish()));
}

SdlDisplay::~SdlDisplay()
{
    delete ui;
}

void SdlDisplay::on_actOpen_triggered()
{
    // 打开文件对话框
    QString title = "打开文件";
    QString dir = QDir::currentPath();
    QString filter = "所有文件(*.*)";
    QString inFileName = QFileDialog::getOpenFileName(this, title, dir, filter);


    // 初始化解码器并显示文件信息
    ui->lineEdit_inFileName->setText(inFileName);

    int ret = m_ffmpegCodec.FFmpegInit(inFileName.toLatin1().data());
    if(0 == ret)
    {
        m_statusLabel->setText("解码器初始化成功");
        statusBar()->addWidget(m_statusLabel);

        int w = m_ffmpegCodec.FFmpegGetVideoW();
        int h = m_ffmpegCodec.FFmpegGetVideoH();
        ui->lineEdit_videoW->setText(QString::number(w));
        ui->lineEdit_videoH->setText(QString::number(h));
        ui->lineEdit_videoEncodeFormat->setText(m_ffmpegCodec.FFmpegGetVideoType());

        ui->widget->setMinimumWidth(w);
        ui->widget->setMinimumHeight(h);

        m_sdl.SdlSetW(w);
        m_sdl.SdlSetH(h);
        m_sdl.SdlSetData(m_ffmpegCodec.FFmpegCodec_GetData());

        ui->actPlay->setEnabled(true);
        ui->actSave->setEnabled(true);

        // 开启线程
        m_timer.SdlTimer_SetStop(0);
    }
    else
    {
        m_statusLabel->setText("解码器初始化失败");
        statusBar()->addWidget(m_statusLabel);

        ui->actPlay->setEnabled(false);
        ui->actSave->setEnabled(false);
    }
}

void SdlDisplay::on_actSave_triggered()
{
    // 选中保存，保存和播放按钮不可选
    ui->actSave->setEnabled(false);
    ui->actPlay->setEnabled(false);

    // 保存文件
    QString title = "保存文件";
    QString dir = QCoreApplication::applicationDirPath();
    QString filter = "所有文件(*.*)";
    QString outFileName = QFileDialog::getSaveFileName(this, title, dir, filter);

    // 设置文件名
    int ret = m_ffmpegCodec.FFmpegCodec_SetSaveFile(outFileName.toLatin1().data());
    if(0 == ret)
    {
        m_statusLabel->setText("开始解码并保存YUV数据");
        statusBar()->addWidget(m_statusLabel);

        QTest::qSleep ( 1000 );

        m_ffmpegCodec.FFmpegCodec_WriteFile();
        m_statusLabel->setText("保存成功，文件名：" + outFileName);
        statusBar()->addWidget(m_statusLabel);
    }
    else
    {
        m_statusLabel->setText("文件打开失败");
        statusBar()->addWidget(m_statusLabel);
    }
}

void SdlDisplay::on_actPlay_triggered()
{
    // 选中保存，保存和播放按钮不可选
    ui->actSave->setEnabled(false);
    ui->actPlay->setEnabled(false);

    int ret = m_sdl.SdlInit((void *)ui->widget->winId());
    if(0 == ret)
    {
        m_statusLabel->setText("正在播放");
        statusBar()->addWidget(m_statusLabel);

//        m_sdl.test(); // 测试sdl显示是否正常

        qDebug() << "addr:" <<m_ffmpegCodec.FFmpegCodec_GetData();

        m_timer.start();
        m_ffmpegCodec.start();
        m_sdl.start();
    }
    else
    {
        m_statusLabel->setText("SDL初始化失败");
        statusBar()->addWidget(m_statusLabel);
    }
}

void SdlDisplay::on_emitDisplayFinish()
{
    qDebug() << "on_emitDisplayFinish";

    m_timer.SdlTimer_SetStop(1);
    m_ffmpegCodec.FFmpegCodec_Free();

    m_statusLabel->setText("播放结束");
    statusBar()->addWidget(m_statusLabel);
}

