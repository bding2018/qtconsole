#include "console_movie.h"
#include "qfontmetrics.h"
#include <QMetaProperty>
#include <qfile.h>
#include <qpainter.h>
#include "qmaskwidget.h"

ConsoleMovie::ConsoleMovie(QWidget* parent) : QLabel(parent)
{
		transparent = 1.0;
		movie = NULL;
        connect(this,SIGNAL(propertyChanged()),this,SLOT(slotPropertyChanged()));
        m_goto_frame =-1;
        m_stop_pix   = "";
        m_movie_path = "";
        m_stoped     = 0;
}

ConsoleMovie::~ConsoleMovie()
{

}

void ConsoleMovie::paintEvent(QPaintEvent *painterEvt)
{

    QPainter painter(this);
	//绘制准备工作,启用反锯齿
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

	QRect rect(0, 0, this->width(), this->height());
	int side = qMin(rect.width(), rect.height());
	qreal scale = side / 200.0;	

	//平移坐标轴中心,等比例缩放
    painter.save();
    //painter.translate(m_localRect.topLeft() - m_initialRect.topLeft());
    painter.translate(rect.center());
    painter.scale(scale, scale);

    drawMovie(&painter);
	//绘制背景

    painter.restore();

}

//鼠标按下事件
void ConsoleMovie::mousePressEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
    QLabel::mousePressEvent(event);
}

//鼠标移动事件
void ConsoleMovie::mouseMoveEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
    QLabel::mouseMoveEvent(event);
}
//鼠标释放事件
void ConsoleMovie::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
    QLabel::mouseReleaseEvent(event);
}

//右键菜单事件
void ConsoleMovie::contextMenuEvent(QContextMenuEvent *event)
{
	Q_UNUSED(event);	
    QLabel::contextMenuEvent(event);
}

//鼠标双击事件
void ConsoleMovie::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    QMaskWidget* mask = dynamic_cast<QMaskWidget*>(parent());
    if(mask)
    {
        mask->shrinkWindow();
    }
    QLabel::mouseDoubleClickEvent(event);
}
void ConsoleMovie::drawMovie(QPainter *painter)
{
    QPixmap p;
    QString s;
	s = QString::fromLocal8Bit("动画无效");
    if (movie && movie->state() == QMovie::Running)//播放状态
	{
        p = movie->currentPixmap();


    }else if(movie && movie->state() == QMovie::Paused)//暂停状态
    {
       	if (QFile::exists(m_stop_pix))
		{
			p = QPixmap(m_stop_pix);
		}
		else
		{
			p = movie->currentPixmap();
		}

    }else if(movie && movie->state() == QMovie::NotRunning)//停止状态
    {
        if(QFile::exists(m_stop_pix))
        {
            p = QPixmap(m_stop_pix);
        }else
        {
            s = QString::fromLocal8Bit("动画已经停止");
        }
    }else //其他状态
	{
        if(QFile::exists(m_stop_pix))
        {
            p = QPixmap(m_stop_pix);
        }else
        {
            s = QString::fromLocal8Bit("没有有效的动画文件");
        }
	}

    if(!p.isNull())
    {
        p = p.scaled(200, 200);
        painter->drawPixmap(-100,-100, p);
    }else
    {
        QPoint p= QPoint(-100,0);
        painter->drawText(p, s);
    }
}

void ConsoleMovie::resetMovie(const QString& file)
{
    if(movie == NULL)
	{
		movie = new QMovie(this);
		movie->setCacheMode(QMovie::CacheAll);
		
		connect(movie, SIGNAL(frameChanged(int)), this, SLOT(slotUpdateFrame()));
		connect(movie, SIGNAL(stateChanged(QMovie::MovieState)),this, SLOT(slotUpdateState()));
    }

    if(movie->state() == QMovie::Running)
	{
		movie->stop();
        m_stoped = true;
	}
	
	if (QFile::exists(file))
	{
		movie->setFileName(file);
		movie->start();
        m_stoped = false;
	}


}

qreal ConsoleMovie::getTransparent()     const
{
	return transparent;
}

void ConsoleMovie::setTransparent(qreal transparent)
{
	if (transparent>1.0)
	{
		transparent = 1.0;
	}
	if (transparent<0)
	{
		transparent = 0;
	}
	this->transparent = transparent;
}

void ConsoleMovie::slotPropertyChanged()
{
 
	if (QFile::exists(m_movie_path) && (m_old_path !=m_movie_path))
	{
		resetMovie(m_movie_path);
		m_old_path = m_movie_path;
        
	}
    if(movie)
    {
        if(m_stoped)
        {
			if(movie->state() == QMovie::Running)
            movie->setPaused(true);
		}else
		{
			if(movie->state()== QMovie::Paused)
			{
				movie->setPaused(false);
			}	
			
			if(movie->state()== QMovie::NotRunning)
			{
				movie->start();
			}
		}



        if( m_goto_frame>=0 && movie->state()!=QMovie::Paused)
        {
           int frame = qAbs(m_goto_frame) % movie->frameCount();
           movie->jumpToFrame(frame);
           movie->setPaused(true);
        }
    }

}

void    ConsoleMovie::setPaused(bool v)
{
    if(movie)
    {
        movie->setPaused(v);
    }
}

//设置播放速度(100为原速度　50% 150%)
void    ConsoleMovie::setSpeed(int v)
{
    int speed = 100;
    if(v>10 && v<1000)
    {
        speed = v;
        if(movie && movie->state()==QMovie::NotRunning)
        {
            movie->start();
            m_stoped = false;
        }
    }


    if(movie)
    {
      movie->setSpeed(v);
    }
}

//动画的播放状态发生变化
void    ConsoleMovie::slotUpdateState()
{
	//qDebug() << "curState=" << movie->state();
    update();
}

void    ConsoleMovie::slotUpdateFrame()
{
    //qDebug() << "frame=" << movie->currentFrameNumber();

    update();
}

//是否处于暂停状态
bool    ConsoleMovie::getPaused()           const
{
    if(movie)
    {
        return (movie->state()==QMovie::Paused);
    }
    return false;
}

//获取当前播放速度
int     ConsoleMovie::getSpeed()            const
{
    if(movie)
    {
        return movie->speed();
    }
    return 100;
}

