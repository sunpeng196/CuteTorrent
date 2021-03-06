﻿
#include <QtGui>
#include "QBaloon.h"
#include <QPropertyAnimation>
#include "StyleEngene.h"
#include <helpers/StaticHelpers.h>

static QBalloonTip* theSolitaryBalloonTip = NULL;

QWidget* QBalloonTip::showBalloon(const QString& title,
                                  const QString& message, QBaloonType type, const QVariant& data, const QSystemTrayIcon::MessageIcon& icon,
                                  int timeout, bool showArrow, QWidget* parent)
{
	theSolitaryBalloonTip = new QBalloonTip(title, message, type, data, icon, parent);
	theSolitaryBalloonTip->balloon(timeout, showArrow);
	return theSolitaryBalloonTip;
}



QBalloonTip::QBalloonTip(const QString& title, const QString& message, QBaloonType type, QVariant data,
                         const QSystemTrayIcon::MessageIcon& icon, QWidget* parent)
	: QWidget(parent, Qt::ToolTip), m_showTimerId(-1)
{
	currentType = type;
	currentData = data;
	setObjectName("QBalloonTip");
	Qt::WindowFlags flags = windowFlags();
	setWindowFlags(flags | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setWindowModality(Qt::NonModal);
	cuurentIcon = icon;
	QLabel* titleLabel = new QLabel;
	titleLabel->installEventFilter(this);
	titleLabel->setText(title);
	QFont f = titleLabel->font();
	f.setBold(true);
	titleLabel->setFont(f);
	titleLabel->setTextFormat(Qt::PlainText);
	QPushButton* closeButton = new QPushButton;
	closeButton->setObjectName("closeBaloon");
	closeButton->setIcon(StyleEngene::getInstance()->getIcon("app_close"));
	closeButton->setIconSize(QSize(18, 18));
	closeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	closeButton->setFixedSize(18, 18);
	connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
	const QRect desctopRect = QApplication::desktop()->geometry();
	setMaximumWidth(desctopRect.width() / 8);
	setMaximumHeight(desctopRect.height() / 8);
	QLabel* msgLabel = new QLabel;
	msgLabel->installEventFilter(this);
	msgLabel->setText(message);
	msgLabel->setTextFormat(Qt::PlainText);
	msgLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	msgLabel->setMaximumWidth(desctopRect.width() / 4);
	msgLabel->setWordWrap(true);
	QIcon si;

	switch(icon)
	{
		case QSystemTrayIcon::Warning:
			si = style()->standardIcon(QStyle::SP_MessageBoxWarning);
			break;

		case QSystemTrayIcon::Critical:
			si = style()->standardIcon(QStyle::SP_MessageBoxCritical);
			break;

		case QSystemTrayIcon::Information:
			si = style()->standardIcon(QStyle::SP_MessageBoxInformation);
			break;

		case QSystemTrayIcon::NoIcon:
		default:
			break;
	}

	QGridLayout* layout = new QGridLayout;

	if(!si.isNull())
	{
		QLabel* iconLabel = new QLabel;
		iconLabel->setPixmap(si.pixmap(15, 15));
		iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		iconLabel->setMargin(2);
		layout->addWidget(iconLabel, 0, 0);
		layout->addWidget(titleLabel, 0, 1);
	}
	else
	{
		layout->addWidget(titleLabel, 0, 0, 1, 2);
	}

	layout->addWidget(closeButton, 0, 2);
	layout->addWidget(msgLabel, 1, 0, 1, 3);
	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->setMargin(3);
	setLayout(layout);
	finished = false;
	/*
		pixmap=QPixmap(":/images/ToolTipFrame.png");
		setMask(pixmap.mask());*/
}

QBalloonTip::~QBalloonTip()
{
	theSolitaryBalloonTip = NULL;
}
/*

void QBalloonTip::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.drawPixmap(rect(), pixmap);
}
*/

void QBalloonTip::resizeEvent(QResizeEvent* ev)
{
	QWidget::resizeEvent(ev);
}

void QBalloonTip::balloon(int msecs, bool showArrow)
{
	QSize sh = sizeHint();
	QRect desktopRect = QApplication::desktop()->availableGeometry();
	QPoint bottomRight = desktopRect.bottomRight();
	const QPoint& pos = mapToGlobal(QPoint(bottomRight.x(), bottomRight.y() /*-sh.height()*/));
	const QPoint& localpos = QPoint(pos);
	const QRect& prect = desktopRect;
	QRect scr = prect;
	const int border = 1;
	const int ah = 18, ao = 18, aw = 18, rc = 7;
	bool arrowAtTop = (localpos.y() - sh.height() - ah < 0);//scr.height());
	bool arrowAtLeft = (localpos.x() + sh.width() - ao < scr.width());
	setContentsMargins(border + 3, border + (arrowAtTop ? ah : 0) + 2, border + 3, border + (arrowAtTop ? 0 : ah) + 2);
	updateGeometry();
	sh = sizeHint();
	int ml, mr, mt, mb;
	QSize sz = sizeHint();

	if(!arrowAtTop)
	{
		ml = mt = 0;
		mr = sz.width() - 1;
		mb = sz.height() - ah - 1;
	}
	else
	{
		ml = 0;
		mt = ah;
		mr = sz.width() - 1;
		mb = sz.height() - 1;
	}

	QPainterPath path;
	path.moveTo(ml + rc, mt);

	if(arrowAtTop && arrowAtLeft)
	{
		if(showArrow)
		{
			path.lineTo(ml + ao, mt);
			path.lineTo(ml + ao, mt - ah);
			path.lineTo(ml + ao + aw, mt);
		}

		move(qMax(pos.x() - ao, scr.left() + 2), pos.y());
	}
	else if(arrowAtTop && !arrowAtLeft)
	{
		if(showArrow)
		{
			path.lineTo(mr - ao - aw, mt);
			path.lineTo(mr - ao, mt - ah);
			path.lineTo(mr - ao, mt);
		}

		move(qMin(pos.x() - sh.width() + ao, scr.right() - sh.width() - 2), pos.y());
	}

	path.lineTo(mr - rc, mt);
	path.arcTo(QRect(mr - rc * 2, mt, rc * 2, rc * 2), 90, -90);
	path.lineTo(mr, mb - rc);
	path.arcTo(QRect(mr - rc * 2, mb - rc * 2, rc * 2, rc * 2), 0, -90);

	if(!arrowAtTop && !arrowAtLeft)
	{
		if(showArrow)
		{
			path.lineTo(mr - ao, mb);
			path.lineTo(mr - ao, mb + ah);
			path.lineTo(mr - ao - aw, mb);
		}

		move(qMin(pos.x() - sh.width() + ao, scr.right() - sh.width() - 2),
		     pos.y() - sh.height());
	}
	else if(!arrowAtTop && arrowAtLeft)
	{
		if(showArrow)
		{
			path.lineTo(ao + aw, mb);
			path.lineTo(ao, mb + ah);
			path.lineTo(ao, mb);
		}

		move(qMax(pos.x() - ao, scr.x() + 2), pos.y() - sh.height());
	}

	path.lineTo(ml + rc, mb);
	path.arcTo(QRect(ml, mb - rc * 2, rc * 2, rc * 2), -90, -90);
	path.lineTo(ml, mt + rc);
	path.arcTo(QRect(ml, mt, rc * 2, rc * 2), 180, -90);

	if(msecs <= 0)
	{
		msecs = 10000;
	}

	m_duration = msecs;
	m_showTimerId = startTimer(msecs);
	show();
	QSequentialAnimationGroup* pAnimationGroup = new QSequentialAnimationGroup(this);
	QPropertyAnimation*  pShowAnimation = new QPropertyAnimation(this, "windowOpacity");
	pShowAnimation->setDuration(msecs / 4);
	pShowAnimation->setStartValue(0.f);
	pShowAnimation->setEndValue(1.f);
	QPropertyAnimation*  pHideAnimation = new QPropertyAnimation(this, "windowOpacity");
	pHideAnimation->setDuration(msecs / 4);
	pHideAnimation->setStartValue(1.f);
	pHideAnimation->setEndValue(0.f);
	pAnimationGroup->addAnimation(pShowAnimation);
	pAnimationGroup->addPause(m_duration / 2);
	pAnimationGroup->addAnimation(pHideAnimation);
	pAnimationGroup->start(QAbstractAnimation::DeleteWhenStopped);
	m_showTimerId = startTimer(m_duration);
}

void QBalloonTip::mousePressEvent(QMouseEvent* e)
{
	switch(currentType)
	{
		case TorrentCompleted :
		{
			QString path = QFileInfo(QDir::toNativeSeparators(currentData.toString())).absoluteFilePath();
#ifdef Q_WS_MAC
			QStringList args;
			args << "-e";
			args << "tell application \"Finder\"";
			args << "-e";
			args << "activate";
			args << "-e";
			args << "select POSIX file \"" + path + "\"";
			args << "-e";
			args << "end tell";
			QProcess::startDetached("osascript", args);
#endif
#ifdef Q_WS_WIN
			StaticHelpers::OpenFileInExplorer(path);
#endif
			break;
		}

		case UpdateNotyfy :
		{
			QDesktopServices::openUrl(QUrl("http://cutetorrent.info/downloads/"));
			break;
		}

		case Error:
		case Info:
		default:
			break;
	}

	e->accept();
	killTimer(m_showTimerId);
	finished = true;
	close();
	return;
}

bool QBalloonTip::isFinished()
{
	return finished;
}

void QBalloonTip::timerEvent(QTimerEvent* e)
{
	if (e->timerId() == m_showTimerId)
	{
		killTimer(m_showTimerId);
		close();
	}
}


/*void QBalloonTip::paintEvent(QPaintEvent* e)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}*/
