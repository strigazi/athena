/////////////////////////////////////////////////////////////////////////
//                                                                     //
// File adopted from KDE4 libraries by                                 //
// T. Kittelmann <Thomas.Kittelmann@cern.ch>, March 2007.              //
//                                                                     //
// Main thing is to remove dependence on KDE for length of title text  //
// settings, delay on drag settings and title eliding. Also,           //
// hoverbuttons were removed (since these had not been properly        //
// implemented in KDE4 at the time the code was copied).               //
//                                                                     //
// Notice about Copyrights and GPL license from the orignal file is    //
// left untouched below.                                               //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

/* This file is part of the KDE libraries
    Copyright (C) 2003 Stephan Binner <binner@kde.org>
    Copyright (C) 2003 Zack Rusin <zack@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QMouseEvent>

#include "QatPresenter/QatTabBar.h"

class QatTabBar::Private
{
  public:
    Private()
      : mReorderStartTab( -1 ),
        mReorderPreviousTab( -1 ),
        mDragSwitchTab( -1 ),
        mActivateDragSwitchTabTimer(nullptr),
        mTabReorderingEnabled( false ),
        mTabCloseActivatePrevious( false )
    {
    }

    QPoint mDragStart;
    int mReorderStartTab;
    int mReorderPreviousTab;
    int mDragSwitchTab;
    QTimer *mActivateDragSwitchTabTimer;

    bool mTabReorderingEnabled;
    bool mTabCloseActivatePrevious;

};

QatTabBar::QatTabBar( QWidget *parent )
    : QTabBar( parent ),
      m_d( new Private )
{
  setAcceptDrops( true );
  setMouseTracking( true );

  //m_d->mEnableCloseButtonTimer = new QTimer( this );
  //connect( m_d->mEnableCloseButtonTimer, SIGNAL( timeout() ), SLOT( enableCloseButton() ) );

  m_d->mActivateDragSwitchTabTimer = new QTimer( this );
  m_d->mActivateDragSwitchTabTimer->setSingleShot( true );
  connect( m_d->mActivateDragSwitchTabTimer, SIGNAL( timeout() ), SLOT( activateDragSwitchTab() ) );

  //connect( this, SIGNAL( layoutChanged() ), SLOT( onLayoutChange() ) );
}

QatTabBar::~QatTabBar()
{
  delete m_d;
}

void QatTabBar::mouseDoubleClickEvent( QMouseEvent *event )
{
  if ( event->button() != Qt::LeftButton )
    return;

  int tab = selectTab( event->pos() );
  if ( tab != -1 ) {
    emit mouseDoubleClick( tab );
    return;
  }

  QTabBar::mouseDoubleClickEvent( event );
}

void QatTabBar::mousePressEvent( QMouseEvent *event )
{
  if ( event->button() == Qt::LeftButton ) {
    //m_d->mEnableCloseButtonTimer->stop();
    m_d->mDragStart = event->pos();
  } else if( event->button() == Qt::RightButton ) {
    int tab = selectTab( event->pos() );
    if ( tab != -1 ) {
      emit contextMenu( tab, mapToGlobal( event->pos() ) );
      return;
    }
  }

  QTabBar::mousePressEvent( event );
}

void QatTabBar::mouseMoveEvent( QMouseEvent *event )
{
  if ( event->buttons() == Qt::LeftButton ) {
    int tab = selectTab( event->pos() );
    if ( m_d->mDragSwitchTab && tab != m_d->mDragSwitchTab ) {
      m_d->mActivateDragSwitchTabTimer->stop();
      m_d->mDragSwitchTab = 0;
    }

    int delay = 5;//TK fixme KGlobalSettings::dndEventDelay();
    QPoint newPos = event->pos();
    if ( newPos.x() > m_d->mDragStart.x() + delay || newPos.x() < m_d->mDragStart.x() - delay ||
         newPos.y() > m_d->mDragStart.y() + delay || newPos.y() < m_d->mDragStart.y() - delay ) {
      if ( tab != -1 ) {
        emit initiateDrag( tab );
        return;
      }
    }
  } else if ( event->buttons() == Qt::MidButton ) {
    if ( m_d->mReorderStartTab == -1 ) {
      int delay = 5;//TK fixme KGlobalSettings::dndEventDelay();
      QPoint newPos = event->pos();

      if ( newPos.x() > m_d->mDragStart.x() + delay || newPos.x() < m_d->mDragStart.x() - delay ||
           newPos.y() > m_d->mDragStart.y() + delay || newPos.y() < m_d->mDragStart.y() - delay ) {
        int tab = selectTab( event->pos() );
        if ( tab != -1 && m_d->mTabReorderingEnabled ) {
          m_d->mReorderStartTab = tab;
          grabMouse( Qt::SizeAllCursor );
          return;
        }
      }
    } else {
      int tab = selectTab( event->pos() );
      if ( tab != -1 ) {
        int reorderStopTab = tab;
        if ( m_d->mReorderStartTab != reorderStopTab && m_d->mReorderPreviousTab != reorderStopTab ) {
          emit moveTab( m_d->mReorderStartTab, reorderStopTab );

          m_d->mReorderPreviousTab = m_d->mReorderStartTab;
          m_d->mReorderStartTab = reorderStopTab;

          return;
        }
      }
    }
  }

  QTabBar::mouseMoveEvent( event );
}


void QatTabBar::activateDragSwitchTab()
{
  int tab = selectTab( mapFromGlobal( QCursor::pos() ) );
  if ( tab != -1 && m_d->mDragSwitchTab == tab )
    setCurrentIndex( m_d->mDragSwitchTab );

  m_d->mDragSwitchTab = 0;
}

void QatTabBar::mouseReleaseEvent( QMouseEvent *event )
{
  if ( event->button() == Qt::MidButton ) {
    if ( m_d->mReorderStartTab == -1 ) {
      int tab = selectTab( event->pos() );
      if ( tab != -1 ) {
        emit mouseMiddleClick( tab );
        return;
      }
    } else {
      releaseMouse();
      setCursor( Qt::ArrowCursor );
      m_d->mReorderStartTab = -1;
      m_d->mReorderPreviousTab = -1;
    }
  }

  QTabBar::mouseReleaseEvent( event );
}

void QatTabBar::dragEnterEvent( QDragEnterEvent *event )
{
  event->setAccepted( true );
  QTabBar::dragEnterEvent( event );
}

void QatTabBar::dragMoveEvent( QDragMoveEvent *event )
{
  int tab = selectTab( event->pos() );
  if ( tab != -1 ) {
    bool accept = false;
    // The receivers of the testCanDecode() signal has to adjust
    // 'accept' accordingly.
    emit testCanDecode( event, accept );
    if ( accept && tab != currentIndex() ) {
      m_d->mDragSwitchTab = tab;
      m_d->mActivateDragSwitchTabTimer->start( QApplication::doubleClickInterval() * 2 );
    }

    event->setAccepted( accept );
    return;
  }

  event->setAccepted( false );
  QTabBar::dragMoveEvent( event );
}

void QatTabBar::dropEvent( QDropEvent *event )
{
  int tab = selectTab( event->pos() );
  if ( tab != -1 ) {
    m_d->mActivateDragSwitchTabTimer->stop();
    m_d->mDragSwitchTab = 0;
    emit receivedDropEvent( tab , event );
    return;
  }

  QTabBar::dropEvent( event );
}

#ifndef QT_NO_WHEELEVENT
void QatTabBar::wheelEvent( QWheelEvent *event )
{
  if ( event->orientation() == Qt::Horizontal )
    return;

  emit( wheelDelta( event->delta() ) );
}
#endif

bool QatTabBar::isTabReorderingEnabled() const
{
  return m_d->mTabReorderingEnabled;
}

void QatTabBar::setTabReorderingEnabled( bool on )
{
  m_d->mTabReorderingEnabled = on;
}

bool QatTabBar::tabCloseActivatePrevious() const
{
  return m_d->mTabCloseActivatePrevious;
}

void QatTabBar::setTabCloseActivatePrevious( bool on )
{
  m_d->mTabCloseActivatePrevious = on;
}


void QatTabBar::tabLayoutChange()
{
  m_d->mActivateDragSwitchTabTimer->stop();
  m_d->mDragSwitchTab = 0;
}

int QatTabBar::selectTab( const QPoint &pos ) const
{
  for ( int i = 0; i < count(); ++i )
    if ( tabRect( i ).contains( pos ) )
      return i;

  return -1;
}

