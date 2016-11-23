﻿/***********************************************************************************
**
** MapObject.h
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#ifndef MAPOBJECT_H
#define MAPOBJECT_H
//----------------------------------------------------------------------------------
#include "../RenderWorldObject.h"
//----------------------------------------------------------------------------------
//Класс объекта карты
class CMapObject : public CRenderWorldObject
{
	//Это тайл воды
	SETGET(bool, IsWaterTile);

public:
	CMapObject(const RENDER_OBJECT_TYPE &type, const uint &serial, const ushort &graphic, const ushort &color, const short &x, const short &y, const char &z);
	virtual ~CMapObject();
};
//----------------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------------
