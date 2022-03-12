#include "ffmpeg_avcodec.h"
#include "ui_ffmpeg_avcodec.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QString>
#include <QDebug>

extern "C"
{
#include "libavcodec/avcodec.h" // avcodec_send_packet
#include "libavcodec/codec.h"   // AVCodec, av_codec_iterate
#include "libavutil/avutil.h"   // AVMediaType
}

#include <iostream>

ffmpeg_avcodec::ffmpeg_avcodec(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ffmpeg_avcodec)
{
    ui->setupUi(this);

    // 窗口大小限制
    setMinimumWidth(1200);
    ui->groupBox->setMaximumHeight(250);
    ui->groupBox_5->setMinimumHeight(350);

    // QComboBox设置控件可输入, 在ui界面设置editable
    // QComboBox设置控件不可插入输入元素，在ui界面insertPolicy为NoInsert
    // QComboBox设置输入可自动搜索补全
    m_pcomp = new QCompleter(ui->comboBox->model(),this);
    ui->comboBox->setCompleter(m_pcomp);
    init_comboBox_codec_enum(AVMEDIA_TYPE_UNKNOWN);

    // 数据显示控件布局
    init_table_view();

}

ffmpeg_avcodec::~ffmpeg_avcodec()
{
    delete ui;
}

void ffmpeg_avcodec::init_comboBox_codec_enum(AVMediaType type)
{
    // 清空下拉菜单
    ui->comboBox->clear();

    // 遍历添加对应类型的解码器
    AVCodec *p_codec, *head = nullptr;
    void *i = 0;
    while(p_codec = (AVCodec*)av_codec_iterate(&i))
    {
        if(head)
        {
            if(p_codec->decode)
            {
                switch (type) {
                case AVMEDIA_TYPE_VIDEO:
                case AVMEDIA_TYPE_AUDIO:
                    if(type == p_codec->type)
                        ui->comboBox->addItem(QString(p_codec->name), p_codec->id);
                    break;
                default:
                    ui->comboBox->addItem(QString(p_codec->name), p_codec->id);
                }
            }
            head->next = p_codec;
        }
        head = p_codec;
    }

    // 下拉框显示为空，0为第一项，-1为空
    // 注意：该设置必须在下拉菜单添加完元素后
    ui->comboBox->setCurrentIndex(-1);
}

void ffmpeg_avcodec::init_table_view()
{
    m_standardItemModel = new QStandardItemModel();

    // 添加表头
    m_standardItemModel->setColumnCount(3);
    //m_standardItemModel->setHeaderData(0, Qt::Horizontal, QStringLiteral("序号"));
    m_standardItemModel->setHeaderData(0, Qt::Horizontal, QStringLiteral("帧类型"));
    m_standardItemModel->setHeaderData(1, Qt::Horizontal, QStringLiteral("大小"));
    m_standardItemModel->setHeaderData(2, Qt::Horizontal, QStringLiteral("解码帧号"));

    // tableView控件
    ui->tableView->setModel(m_standardItemModel);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);  // 表格自适应列宽
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);    // 表格自适应行宽
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows); // 设置选中时选中整行
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 表格单元为只读，不可编辑
    //ui->tableView->verticalHeader()->hide();    // 隐藏默认显示的行头，即跟表头一个颜色，每行开头会有的依次递增的序号
    ui->tableView->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Fixed);  //设定表头列宽不可变
    ui->tableView->horizontalHeader()->setSectionResizeMode(1,QHeaderView::Fixed);
    ui->tableView->horizontalHeader()->setSectionResizeMode(2,QHeaderView::Stretch);//设定第2列表头弹性拉伸

    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);   // 设置垂直滚动条一直显示，默认为自动显示
}

void ffmpeg_avcodec::setTextCursorToTop()
{
    QTextCursor textCursor = ui->textEdit->textCursor();
    textCursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    ui->textEdit->setTextCursor(textCursor);
}

void ffmpeg_avcodec::test_avcodec_perser()
{
    // 分配m_avcodec_Context并简单初始化
    m_avcodec_Context = avcodec_alloc_context3(m_avcodec);
    if(nullptr == m_avcodec_Context)
    {
        std::cout << "AVCodecContext is nullper!" << std::endl;
        return;
    }
    // 根据m_avcodec初始化m_avcodec_Context
    if(avcodec_open2(m_avcodec_Context, m_avcodec, nullptr) < 0)
    {
        std::cout << "Initialize the AVCodecContext error!" << std::endl;
        return;
    }


    // 根据m_avcodec初始化解析器
    m_avcodec_ParsetContext = av_parser_init(m_avcodec->id);
    if(nullptr == m_avcodec_ParsetContext)
    {
        std::cout << "AVCodecParserContext is nullper!" << std::endl;
        return;
    }

    // 打开文件
    char* p_in_file = m_infile_name.toLatin1().data();
    m_infile.open(p_in_file, std::ios::in | std::ios::binary);
    if(!m_infile)
    {
        std::cout << "in file:" << p_in_file<< " open failed!" << std::endl;
        return;
    }
    m_outfile.open(m_outfile_name.toLatin1().data(), std::ios::out | std::ios::binary);
    if(!m_outfile)
    {
        std::cout << "out file:" << p_in_file<< " open failed!" << std::endl;
        return;
    }

    // 帧数据
    int index = 0;
    AVPacket *pPacket;    // 压缩的数据
    pPacket = av_packet_alloc(); // 可以代替以前版本的av_init_packet()
    AVFrame *pFrame;     // 解码后的数据
    pFrame = av_frame_alloc();

    const int in_buf_size = 4096 + AV_INPUT_BUFFER_PADDING_SIZE;               // 一次性从文件中读出的最大长度
    char *in_buf_ptr = new char[in_buf_size];   //  读入数据存放的数组

    int real_read_size = 0;                     // 每次循环读入
    char *cur_ptr = nullptr;
    int frame_seek_file = 0;

    while (1) {
        // 从文件中读入数据
        real_read_size = m_infile.readsome(in_buf_ptr,in_buf_size);
        if(real_read_size <= 0)
            break;
        cur_ptr = in_buf_ptr;

        // 组帧
        while(real_read_size > 0)
        {
            int len = av_parser_parse2(m_avcodec_ParsetContext, m_avcodec_Context,
                                       &pPacket->data, &pPacket->size,
                                       (unsigned char*)cur_ptr, real_read_size,
                                       AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
            cur_ptr += len;
            real_read_size -= len;

            if(0 == pPacket->size)   // 如果该值为0，表示为不完整帧，需要继续解析
                continue;

            QString picture_type;
            switch(m_avcodec_ParsetContext->pict_type){
                case AV_PICTURE_TYPE_I: picture_type = "I";break;
                case AV_PICTURE_TYPE_P: picture_type = "P";break;
                case AV_PICTURE_TYPE_B: picture_type = "B";break;
                default: picture_type = "Type:Other";break;
            }

            // table view
            // 第0列
            m_standardItemModel->setItem(index, 0, new QStandardItem(picture_type));
            m_standardItemModel->item(index,0)->setTextAlignment(Qt::AlignCenter);
            // 第1列
            //m_standardItemModel->setItem(index, 1, new QStandardItem(QString::number(pPacket->size)));
            m_standardItemModel->setItem(index, 1, new QStandardItem(tr("%1").arg(pPacket->size)));
            m_standardItemModel->item(index,1)->setTextAlignment(Qt::AlignCenter);
            // 第2列
            m_standardItemModel->setItem(index, 2, new QStandardItem(tr("%1").arg(m_avcodec_ParsetContext->output_picture_number)));
            m_standardItemModel->item(index,2)->setTextAlignment(Qt::AlignCenter);

            index++;

            // textEdit
            CompressedFrameData m(frame_seek_file, pPacket->size);
            m_frame_info.push_back(m);
            frame_seek_file += pPacket->size;

            // 解码
            int ret = avcodec_send_packet(m_avcodec_Context, pPacket);  // 送入一帧进行解码
            if(0 == ret)
            {
                ret = avcodec_receive_frame(m_avcodec_Context, pFrame); // 获取一帧解码数据
                if(0 == ret)
                {
                    for(int row = 0; row < pFrame->height; ++row)
                        m_outfile.write((char*)(pFrame->data[0] + pFrame->linesize[0] * row), pFrame->width);
                    for(int row = 0; row < pFrame->height / 2; ++row)
                        m_outfile.write((char*)(pFrame->data[1] + pFrame->linesize[1] * row), pFrame->width / 2);
                    for(int row = 0; row < pFrame->height / 2; ++row)
                        m_outfile.write((char*)(pFrame->data[2] + pFrame->linesize[2] * row), pFrame->width / 2);

                }
            }
        }
    }

    m_infile.close();
    m_outfile.close();

    std::cout << "ok" << std::endl;
}



void ffmpeg_avcodec::on_pushButton_clicked()
{
    m_infile_name = QFileDialog::getOpenFileName(this, tr("文本对话框"));
    ui->lineEdit_in->setText(m_infile_name);
}

void ffmpeg_avcodec::on_pbt_parse_clicked()
{
    // 文件及解码器选择
    if("" == m_infile_name)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("请先选择带解码文件！"));
        msgBox.exec();
        return;
    }

    if("" == m_outfile_name)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("请先确定待保存文件名！"));
        msgBox.exec();
        return;
    }

    if(nullptr == m_avcodec)
    {
        QMessageBox msgBox;
        msgBox.setText(tr("请先选择解码类型！"));
        msgBox.exec();
        return;
    }

    // 解析
    test_avcodec_perser();
}


void ffmpeg_avcodec::on_radioButton_clicked(bool checked)
{
    init_comboBox_codec_enum(AVMEDIA_TYPE_VIDEO);

}

void ffmpeg_avcodec::on_radioButton_2_clicked(bool checked)
{
    init_comboBox_codec_enum(AVMEDIA_TYPE_AUDIO);
}

static AVCodec *test_set_avcodec(char* name)
{
    return avcodec_find_decoder_by_name(name);
}

void ffmpeg_avcodec::on_comboBox_currentIndexChanged(const QString &arg1)
{
    // 这里会发现每次QComboBox执行init_comboBox_codec_enum都会2次触发on_comboBox_currentIndexChanged，Index会先下发0再下方-1，
    //std::cout << "Index = " << ui->comboBox->currentIndex() << std::endl;

    char* tmp = arg1.toLatin1().data();
    //std::cout << "str = " << tmp<< std::endl;
    if(-1 == ui->comboBox->currentIndex())
    {
        m_avcodec = nullptr;
        return;
    }

    //m_codec_id = ui->comboBox->currentData().toInt();
    //std::cout << "m_codec_id = " << m_codec_id << std::endl;

    // 根据name找到codec
    m_avcodec = test_set_avcodec(tmp);
}

ffmpeg_avcodec::CompressedFrameData::CompressedFrameData(int s, int l) : start(s), length(l){};

void ffmpeg_avcodec::on_tableView_clicked(const QModelIndex &index)
{
    std::cout << "row = " << index.row() << std::endl;
    CompressedFrameData tmp = m_frame_info[index.row()];

    ui->textEdit->clear();
    ui->textEdit->setLineWrapMode(QTextEdit::FixedColumnWidth); // 搭配setLineWrapColumnOrWidth使用
    ui->textEdit->setLineWrapColumnOrWidth((16 + 15) * 2);      // 按照十六进制一行（16个数+中间"  "间隔）* 2
    //ui->textEdit->setFontPointSize(12); // 需要放置在setText之前，否则不生效

    // 打开文件
    QFile file1(m_infile_name);
    QDataStream in(&file1);
    file1.open(QIODevice::ReadOnly);
    file1.seek(tmp.start);
    std::cout << "start pos: " << file1.pos() << std::endl;

    // 从指定位置读取并显示
    QByteArray ba;
    int cnt = 0;

    int byte;
    while(cnt < tmp.length && file1.read((char*)&byte, 1))  // 使用char byte;会有某些数出现FFFFFFFF+byte这种，换成int后就没有了，原因不明
    {     
        ba += QString("%1").arg(byte, 2, 16, QLatin1Char('0')).toUpper();   // 将byte以十六进制大写显示，2位，高位0填充
        ba += "  ";
        cnt++;
    }

    ui->textEdit->append(ba);
    setTextCursorToTop();   // 填充完数据后调用才能生效

    file1.close();
}

void ffmpeg_avcodec::on_pushButton_out_clicked()
{
    QString curPath = QCoreApplication::applicationDirPath();   //本路径不可修改
    QString dlgTitle = "保存文件为";
    QString filter = "所有格式(*.*)";
    m_outfile_name = QFileDialog::getSaveFileName(this, dlgTitle, curPath, filter); // 这里注意输入文件名时要自己加后缀，否则会按照filter默认加，当默认为所有格式时，不加后缀会报错
    ui->lineEdit_out->setText(m_outfile_name);
}
