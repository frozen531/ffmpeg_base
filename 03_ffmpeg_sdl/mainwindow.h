#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ffmpegcodec.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actOpenFile_triggered();

    void on_actSaveFile_triggered();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    QString m_infilename;
    QString m_outfilename;

    FFmpegCodec m_ffmpegcodec;
};

#endif // MAINWINDOW_H
