#ifndef FFMPEG_AVCODEC_H
#define FFMPEG_AVCODEC_H

#include <QMainWindow>
#include <QFile>
#include <QCompleter>
#include <QStandardItemModel>

extern "C"
{
/* FFMPEG头文件 */
#include "libavcodec/codec.h"   // AVCodec AVMediaType
#include "libavcodec/avcodec.h" // AVCodecContext AVCodecParserContext
}

#include <fstream>
#include <vector>

namespace Ui {
class ffmpeg_avcodec;
}

class ffmpeg_avcodec : public QMainWindow
{
    Q_OBJECT

public:
    explicit ffmpeg_avcodec(QWidget *parent = nullptr);
    ~ffmpeg_avcodec();

private slots:
    /**
     * @brief on_pushButton_clicked
     * @attention 打开文件按钮
     */
    void on_pushButton_clicked();

    /**
     * @brief on_pbt_parse_clicked
     * @attention 开始解析文件，需要打开文件并选择对应格式后才能开始解析
     */
    void on_pbt_parse_clicked();

    /**
     * @brief on_radioButton_clicked
     * @param checked
     * @attention 文件格式为视频，初始化下拉菜单
     */
    void on_radioButton_clicked(bool checked);

    /**
     * @brief on_radioButton_2_clicked
     * @param checked
     * @attention 文件格式为音频，初始化下拉菜单
     */
    void on_radioButton_2_clicked(bool checked);

    /**
     * @brief on_comboBox_currentIndexChanged
     * @param arg1
     * @attention 组合框选择，并根据id或name找到avcodec
     */
    void on_comboBox_currentIndexChanged(const QString &arg1);

    /**
     * @brief on_tableView_clicked
     * @param index
     * @attention tableView槽函数
     */
    void on_tableView_clicked(const QModelIndex &index);

    /**
     * @brief on_pushButton_out_clicked
     * @attention 选择输出文件名
     */
    void on_pushButton_out_clicked();

private:
    Ui::ffmpeg_avcodec *ui;

    /**
     * @brief init_comboBox_codec_enum
     * @param type
     * @attention 组合框填充项
     */
    void init_comboBox_codec_enum(AVMediaType type);

    /**
     * @brief init_table_view
     * @attention 表格数据展示界面显示
     */
    void init_table_view();

    /**
     * @brief setTextCursorToTop
     * @attention QTextEdit当内容超出显示框时，滚动条置顶
     */
    void setTextCursorToTop();

    class CompressedFrameData
    {
    public:
        CompressedFrameData(int s, int l);
        int start;      // 帧数据起始位置
        int length;     // 帧数据长度
    };
    std::vector<CompressedFrameData> m_frame_info;

    /**
     * @brief test_avcodec_perser
     * @attention 解析
     */
    void test_avcodec_perser();

    // 界面相关
    QCompleter              *m_pcomp;                               // 用于组合框QComboBox输入补全
    QStandardItemModel      *m_standardItemModel;                   // 用于数据可视组件的model_table_Model
    QStandardItemModel      *m_standardItemModel_column;            // 用于数据可视组件的model_column

    // 文件相关
    QString                 m_infile_name;
    QString                 m_outfile_name;
    std::ifstream           m_infile;
    std::ofstream           m_outfile;

    // 解码相关
    AVCodec                 *m_avcodec                  = nullptr;  // 解码器
    AVCodecContext          *m_avcodec_Context          = nullptr;
    AVCodecParserContext    *m_avcodec_ParsetContext    = nullptr;

};

#endif // FFMPEG_AVCODEC_H
