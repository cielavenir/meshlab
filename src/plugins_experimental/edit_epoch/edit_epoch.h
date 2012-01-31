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

#ifndef EditEpochPLUGIN_H
#define EditEpochPLUGIN_H

#include <QObject>
#include <common/interfaces.h>
#include "epoch_io.h"



class EditEpochPlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
	
	enum 
	{
		ALIGN_IDLE = 0x01,
		ALIGN_INSPECT_ARC = 0x02,
		ALIGN_MOVE = 0x03
	};
		
	void Pick( MeshModel &m, GLArea * gla);

public:
    EditEpochPlugin();
		virtual ~EditEpochPlugin() {
			mode=ALIGN_IDLE;
		}

    static const QString Info();

    bool StartEdit(MeshDocument &/*m*/, GLArea * /*parent*/);
    void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    void Decorate(MeshModel &/*m*/, GLArea * /*parent*/);
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) ;
    void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );

		QFont qFont;
  	int mode;			

    v3dImportDialog *epochDialog; //the widget with the list of the meshes. 
		
		void toggleButtons();

		
		
		MeshDocument *md; 
		
    	GLArea * gla;

public:
	
	
	// this callback MUST be redefined because we are able to manage internally the layer change.
  virtual void LayerChanged(MeshDocument &/*md*/, MeshModel &/*oldMeshModel*/, GLArea */*parent*/)
	{ 
	// add code here to manage the external layer switching
	}

public slots:
		void process();
		void recalcCurrentArc();
		void glueHere();
		void glueHereAll();
		void glueManual();
		void glueByPicking();
		void alignParam();
		void alignParamCurrent();
    void setBaseMesh();
    void hideRevealGluedMesh();
		
		
signals:
	void suspendEditToggle();

};

#endif
