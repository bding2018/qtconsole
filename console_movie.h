#ifndef CUSTOM_CONSOLE_MOVIE_H
#define CUSTOM_CONSOLE_MOVIE_H
#include "qmovie.h"
#include "qlabel.h"
/**************************************************************
 *类名:　ConsoleMovie
 *基类:　QLabel
 *描述:　高级动画功能
 *作者:　bding@wiscom.com.cn
 *创建: 2022.11.03
 *************************************************************/
class ConsoleMovie: public QLabel
{
	Q_OBJECT	
    Q_PROPERTY(QString moviePath  MEMBER m_movie_path NOTIFY propertyChanged)
    Q_PROPERTY(QString stopPix    MEMBER m_stop_pix)
    Q_PROPERTY(int    goFrame     MEMBER m_goto_frame NOTIFY propertyChanged)
	Q_PROPERTY(qreal  transparent READ  getTransparent  WRITE setTransparent)
    Q_PROPERTY(bool   stoped      MEMBER m_stoped NOTIFY propertyChanged)
    Q_PROPERTY(bool   paused      READ  getPaused       WRITE setPaused)
	Q_PROPERTY(int    speed       READ  getSpeed        WRITE setSpeed)
public:
    ConsoleMovie(QWidget *parent = 0);
    ~ConsoleMovie();
    void resetMovie(const QString& file);
protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    void contextMenuEvent(QContextMenuEvent *event) Q_DECL_OVERRIDE;
	void drawMovie(QPainter *painter);
protected:
    QString m_movie_path;
	QString m_old_path;
    QString m_stop_pix;
    int     m_goto_frame=-1;

    qreal  transparent;
    bool   m_stoped=false;
	QMovie *movie=NULL;
public:
	qreal   getTransparent()      const;
	bool    getPaused()           const;
	int     getSpeed()            const;
public Q_SLOTS:
	//设置背景颜色+文字颜色+高亮颜色+标识颜色
	void setTransparent(qreal v);
    void setPaused(bool v);
    void setSpeed(int v);
	void slotUpdateFrame();
	void slotUpdateState();
    void slotPropertyChanged();

signals:
    void propertyChanged();
};
#endif // CUSTOM_CONSOLE_MOVIE_H
