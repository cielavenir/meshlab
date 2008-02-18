/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/
/****************************************************************************
History
Log: qualityMapper.cpp
Revision 1.0 2008/01/03 MAL
Beginning

****************************************************************************/

#include <QtGui>
#include <limits>
#include <meshlab/glarea.h>
#include "qualitymapper.h"
#include <vcg/complex/trimesh/clean.h>

#include <wrap/gl/pick.h>
//#include <wrap/qt/trackball.h>

#include <vcg/complex/trimesh/update/flag.h>
// #include "color_manifold.h"

//#include "../../meshlabplugins/meshcolorize/equalizerDialog.h"	//for EqualizerSettings
// #include "smoothcolor.h"
#include <vcg/space/triangle3.h> //for quality

//using namespace std;
using namespace vcg;

QualityMapperPlugin::QualityMapperPlugin()
{
	actionList << new QAction(QIcon(":/images/qualitymapper.png"),"Quality Mapper. More info coming soon...", this);
	QAction *editAction;
	foreach(editAction, actionList)
		editAction->setCheckable(true);

	_qualityMapperDialog = 0;
}

QList<QAction *> QualityMapperPlugin::actions() const
{	return actionList;	}


const QString QualityMapperPlugin::Info(QAction *action) 
{
	if( action->text() != tr("Get Info") ) assert (0);

	return tr("Colorize mesh vertexes by Quality following some rules");
}

const PluginInfo &QualityMapperPlugin::Info() 
{
	static PluginInfo ai; 
	ai.Date=tr("Jan 2008");
	ai.Version = tr("1.0");
	ai.Author = ("Alessandro Maione, Federico Bellucci");
	return ai;
} 


void QualityMapperPlugin::mouseReleaseEvent  (QAction *,QMouseEvent *, MeshModel &, GLArea *)
{
}

void QualityMapperPlugin::mousePressEvent    (QAction *, QMouseEvent *, MeshModel &, GLArea *)
{  
}

void QualityMapperPlugin::mouseMoveEvent     (QAction *,QMouseEvent *, MeshModel &, GLArea *)
{
}

void QualityMapperPlugin::Decorate(QAction *, MeshModel&, GLArea *)
{
}

void QualityMapperPlugin::StartEdit(QAction *mode, MeshModel &m, GLArea *gla )
{
	//	gla->setCursor(QCursor(QPixmap(":/images/cur_info.png"),1,1));	

	if(_qualityMapperDialog==0)
		_qualityMapperDialog = new QualityMapperDialog(gla->window(), &m, gla);

	//drawing histogram
	//bool ret = _qualityMapperDialog->initEqualizerHistogram();
	if ( !_qualityMapperDialog->initEqualizerHistogram() )
	{
		EndEdit(mode, m, gla);
		return;
	}


	//drawing transferFunction
	_qualityMapperDialog->drawTransferFunction();



	//dialog ready to be displayed. Show it now!
	_qualityMapperDialog->show();

	connect(_qualityMapperDialog, SIGNAL(closingDialog()),gla,SLOT(endEdit()) );
}

void QualityMapperPlugin::EndEdit(QAction * , MeshModel &, GLArea * )
{
	if ( _qualityMapperDialog )
	{
		_qualityMapperDialog->disconnect();
		delete _qualityMapperDialog;
		_qualityMapperDialog = 0;
	}
}


Q_EXPORT_PLUGIN(QualityMapperPlugin)

