/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
//Autogenerated VP1 Factory Code Header File (Fri Aug 19 13:36:52 CEST 2016)

#ifndef VP1GeometryPlugin_VP1AutoFactory_H
#define VP1GeometryPlugin_VP1AutoFactory_H

#include <QObject>
#include <QStringList>

#include "VP1Base/IVP1ChannelWidgetFactory.h"

class VP1GeometryPlugin_VP1AutoFactory : public QObject, public IVP1ChannelWidgetFactory
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "VP1GeometryPlugin" )
  Q_INTERFACES(IVP1ChannelWidgetFactory)

public:
  virtual QStringList channelWidgetsProvided() const;
  virtual IVP1ChannelWidget * getChannelWidget(const QString & channelwidget);
};

#endif
