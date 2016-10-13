/***********************************************************************************
**
** GumpSpell.cpp
**
** Copyright (C) August 2016 Hotride
**
************************************************************************************
*/
//----------------------------------------------------------------------------------
#include "GumpSpell.h"
#include "../SelectedObject.h"
#include "../OrionUO.h"
#include "../Managers/MouseManager.h"
#include "../Managers/ClilocManager.h"
#include "../Managers/GumpManager.h"
#include "../Managers/ConfigManager.h"
#include "../ToolTip.h"
#include "../PressedObject.h"
//----------------------------------------------------------------------------------
CGumpSpell::CGumpSpell(uint serial, short x, short y, ushort graphic)
: CGump(GT_SPELL, serial, x, y), m_GroupNext(NULL), m_GroupPrev(NULL)
{
	m_Graphic = graphic;
	m_Locker.Serial = ID_GS_LOCK_MOVING;

	m_Blender = (CGUIAlphaBlending*)Add(new CGUIAlphaBlending(g_ConfigManager.TransparentSpellIcons, g_ConfigManager.SpellIconAlpha / 255.0f));
	Add(new CGUIGumppic(m_Graphic, 0, 0));

	m_SpellUnlocker = (CGUIButton*)Add(new CGUIButton(ID_GS_BUTTON_REMOVE_FROM_GROUP, 0x082C, 0x082C, 0x082C, 30, 16));
	m_SpellUnlocker->CheckPolygone = true;
	m_SpellUnlocker->Visible = false;

	Add(new CGUIAlphaBlending(false, 0.0f));
}
//----------------------------------------------------------------------------------
CGumpSpell::~CGumpSpell()
{
	RemoveFromGroup();
}
//----------------------------------------------------------------------------------
void CGumpSpell::InitToolTip()
{
	uint id = g_SelectedObject.Serial;

	if (id == ID_GS_BUTTON_REMOVE_FROM_GROUP)
		g_ToolTip.Set(L"Remove spell from group", g_SelectedObject.Object(), 80);
	else
		g_ToolTip.Set(g_ClilocManager.Cliloc(g_Language)->GetW(3002010 + m_Serial), g_SelectedObject.Object(), 80);
}
//----------------------------------------------------------------------------------
void CGumpSpell::PrepareContent()
{
	if (!g_ConfigManager.TransparentSpellIcons)
	{
		if (m_Blender->Enabled)
		{
			m_Blender->Enabled = false;
			m_WantRedraw = true;
		}
	}
	else if (g_SelectedObject.Gump() == this)
	{
		if (m_Blender->Enabled)
		{
			m_Blender->Enabled = false;
			m_WantRedraw = true;
		}
	}
	else if (!m_Blender->Enabled)
	{
		m_Blender->Enabled = true;
		m_WantRedraw = true;
	}
}
//----------------------------------------------------------------------------------
CGumpSpell *CGumpSpell::GetTopSpell()
{
	if (!InGroup())
		return NULL;

	if (m_GroupPrev == NULL)
		return this;

	CGumpSpell *gump = m_GroupPrev;

	while (gump != NULL && gump->m_GroupPrev != NULL)
		gump = gump->m_GroupPrev;

	return gump;
}
//----------------------------------------------------------------------------------
CGumpSpell *CGumpSpell::GetNearSpell(int &x, int &y)
{
	if (InGroup())
		return NULL;

	static const int gumpWidth = 44;
	static const int gumpHeight = 44;

	const int rangeX = 22;
	const int rangeY = 22;
	const int rangeOffsetX = 30;
	const int rangeOffsetY = 30;

	CGump *gump = (CGump*)g_GumpManager.m_Items;

	while (gump != NULL)
	{
		if (gump != this && gump->GumpType == GT_SPELL)
		{
			int gumpX = gump->X;
			int offsetX = abs(x - gumpX);
			int passed = 0;

			if (x >= gumpX && x <= (gumpX + gumpWidth))
				passed = 2;
			else if (offsetX < rangeOffsetX) //left part of gump
				passed = 1;
			else
			{
				offsetX = abs(x - (gumpX + gumpWidth));

				if (offsetX < rangeOffsetX) //right part of gump
					passed = -1;
				else if (x >= (gumpX - rangeX) && x <= (gumpX + gumpWidth + rangeX))
					passed = 2;
			}

			int gumpY = gump->Y;

			if (abs(passed) == 1)
			{
				if (y < (gumpY - rangeY) || y >(gumpY + gumpHeight + rangeY))
					passed = 0;
			}
			else if (passed == 2) //in gump range X
			{
				int offsetY = abs(y - gumpY);

				if (offsetY < rangeOffsetY) //top part of gump
					passed = 2;
				else
				{
					offsetY = abs(y - (gumpY + gumpHeight));

					if (offsetY < rangeOffsetY) //bottom part of gump
						passed = -2;
					else
						passed = 0;
				}
			}

			if (passed)
			{
				int testX = gumpX;
				int testY = gumpY;

				switch (passed)
				{
					case -2: //gump bottom
					{
						testY += gumpHeight;
						break;
					}
					case -1: //gump right
					{
						testX += gumpWidth;
						break;
					}
					case 1: //gump left
					{
						testX -= gumpWidth;
						break;
					}
					case 2: //gump top
					{
						testY -= gumpHeight;
						break;
					}
					default:
						break;
				}

				CGump *testGump = (CGump*)g_GumpManager.m_Items;

				while (testGump != NULL)
				{
					if (testGump != this && testGump->GumpType == GT_SPELL)
					{
						if (testGump->X == testX && testGump->Y == testY)
							break;
					}

					testGump = (CGump*)testGump->m_Next;
				}

				if (testGump == NULL)
				{
					x = testX;
					y = testY;

					break;
				}
			}
		}

		gump = (CGump*)gump->m_Next;
	}

	return (CGumpSpell*)gump;
}
//----------------------------------------------------------------------------------
bool CGumpSpell::GetSpellGroupOffset(int &x, int &y)
{
	if (InGroup() && g_MouseManager.LeftButtonPressed && g_PressedObject.LeftGump() != NULL)
	{
		CGumpSpell *gump = GetTopSpell();

		while (gump != NULL)
		{
			//���� ���� ��������� � (����� ����) �������
			if (gump != this && g_PressedObject.LeftGump() == gump && gump->CanBeMoved())
			{
				WISP_GEOMETRY::CPoint2Di offset = g_MouseManager.LeftDroppedOffset();

				x += offset.X;
				y += offset.Y;

				return true;
			}

			gump = gump->m_GroupNext;
		}
	}

	return false;
}
//----------------------------------------------------------------------------------
void CGumpSpell::UpdateGroup(int x, int y)
{
	if (!InGroup())
		return;

	CGumpSpell *gump = GetTopSpell();

	while (gump != NULL)
	{
		if (gump != this)
		{
			gump->X += x;
			gump->Y += y;

			g_GumpManager.MoveToBack(gump);
			//gump->WantRedraw = true;
		}

		gump = gump->m_GroupNext;
	}

	g_GumpManager.MoveToBack(this);
}
//----------------------------------------------------------------------------------
void CGumpSpell::AddSpell(CGumpSpell *spell)
{
	if (m_GroupNext == NULL)
	{
		m_GroupNext = spell;
		spell->m_GroupPrev = this;
		spell->m_GroupNext = NULL;
	}
	else
	{
		CGumpSpell *gump = m_GroupNext;

		while (gump != NULL && gump->m_GroupNext != NULL)
			gump = gump->m_GroupNext;

		gump->m_GroupNext = spell;
		spell->m_GroupPrev = gump;
		spell->m_GroupNext = NULL;
	}

	if (spell->m_SpellUnlocker != NULL)
	{
		spell->m_SpellUnlocker->Visible = spell->InGroup();
		spell->WantRedraw = true;
	}

	if (m_SpellUnlocker != NULL)
	{
		m_SpellUnlocker->Visible = InGroup();

		m_WantRedraw = true;
	}
}
//----------------------------------------------------------------------------------
void CGumpSpell::RemoveFromGroup()
{
	if (m_GroupNext != NULL)
	{
		m_GroupNext->WantRedraw = true;
		m_GroupNext->m_GroupPrev = m_GroupPrev;

		if (m_GroupNext->m_SpellUnlocker != NULL)
			m_GroupNext->m_SpellUnlocker->Visible = m_GroupNext->InGroup();
	}

	if (m_GroupPrev != NULL)
	{
		m_GroupPrev->WantRedraw = true;
		m_GroupPrev->m_GroupNext = m_GroupNext;

		if (m_GroupPrev->m_SpellUnlocker != NULL)
			m_GroupPrev->m_SpellUnlocker->Visible = m_GroupPrev->InGroup();
	}

	m_GroupNext = NULL;
	m_GroupPrev = NULL;

	if (m_SpellUnlocker != NULL)
	{
		m_SpellUnlocker->Visible = InGroup();
		m_WantRedraw = true;
	}
}
//----------------------------------------------------------------------------------
void CGumpSpell::CalculateGumpState()
{
	CGump::CalculateGumpState();

	//���� ���� ��������� � (����� ����) �������
	if (g_GumpMovingOffset.X || g_GumpMovingOffset.Y)
	{
		if (!InGroup())
		{
			int testX = g_MouseManager.Position.X;
			int testY = g_MouseManager.Position.Y;

			if (GetNearSpell(testX, testY) != NULL)
			{
				g_GumpTranslate.X = (float)testX;
				g_GumpTranslate.Y = (float)testY;
			}
		}
	}
	else if (InGroup())
	{
		int x = (int)g_GumpTranslate.X;
		int y = (int)g_GumpTranslate.Y;

		GetSpellGroupOffset(x, y);

		g_GumpTranslate.X = (float)x;
		g_GumpTranslate.Y = (float)y;
	}
}
//----------------------------------------------------------------------------------
void CGumpSpell::GUMP_BUTTON_EVENT_C
{
	if (serial == ID_GS_LOCK_MOVING)
		m_LockMoving = !m_LockMoving;
	else if (serial == ID_GS_BUTTON_REMOVE_FROM_GROUP)
	{
		CGumpSpell *oldGroup = m_GroupNext;

		if (oldGroup == NULL)
			oldGroup = m_GroupPrev;

		RemoveFromGroup();

		if (oldGroup != NULL)
		{
			oldGroup->UpdateGroup(0, 0);
			oldGroup->WantRedraw = true;
		}
	}
}
//----------------------------------------------------------------------------------
bool CGumpSpell::OnLeftMouseButtonDoubleClick()
{
	g_Orion.CastSpell(m_Serial);

	return true;
}
//----------------------------------------------------------------------------------
