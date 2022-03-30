#ifndef SDLTIMER_H
#define SDLTIMER_H

#include <QThread>

class SdlTimer : public QThread
{
    Q_OBJECT
public:
    explicit SdlTimer();

    void run() Q_DECL_OVERRIDE;

    void SdlTimer_SetStop(int stop){m_time_stop = stop;}
signals:
    void emit_decode();

public slots:

private:
    int m_time_stop = false;
};

#endif // SDLTIMER_H
