/*
* This file is part of wxSmith plugin for Code::Blocks Studio
* Copyright (C) 2006-2007  Bartlomiej Swiecki
*
* wxSmith is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* wxSmith is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with wxSmith. If not, see <http://www.gnu.org/licenses/>.
*
* $Revision: 13235 $
* $Id: wxschoice.cpp 13235 2023-03-15 14:00:15Z wh11204 $
* $HeadURL: svn://svn.code.sf.net/p/codeblocks/code/trunk/src/plugins/contrib/wxSmith/wxwidgets/defitems/wxschoice.cpp $
*/

#include <wx/choice.h>
#include "wxschoice.h"

namespace
{
    wxsRegisterItem<wxsChoice> Reg(_T("Choice"),wxsTWidget,_T("Standard"),310);

    WXS_ST_BEGIN(wxsChoiceStyles,_T(""))
        WXS_ST_CATEGORY("wxChoice")
        WXS_ST(wxCB_SORT)
        WXS_ST_DEFAULTS()
    WXS_ST_END()


    WXS_EV_BEGIN(wxsChoiceEvents)
        WXS_EVI(EVT_CHOICE,wxEVT_COMMAND_CHOICE_SELECTED,wxCommandEvent,Select)
    WXS_EV_END()
}

wxsChoice::wxsChoice(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsChoiceEvents,
        wxsChoiceStyles),
    DefaultSelection(-1)
{}


void wxsChoice::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/choice.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, %P, %S, 0, 0, %T, %V, %N);\n"));

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( DefaultSelection == (int)i )
                {
                    Codef(_T("%ASetSelection( "));
                }
                Codef(_T("%AAppend(%t)"),ArrayChoices[i].wx_str());
                if ( DefaultSelection == (int)i )
                {
                    Codef(_T(" )"));
                }
                Codef(_T(";\n"));
            }

            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsChoice::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsChoice::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxChoice* Preview = new wxChoice(Parent,GetId(),Pos(Parent),Size(Parent),0,0,Style());

    for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
    {
        int Val = Preview->Append(ArrayChoices[i]);
        if ( (int)i == DefaultSelection )
        {
            Preview->SetSelection(Val);
        }
    }
    return SetupWindow(Preview,Flags);
}

void wxsChoice::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_ARRAYSTRING(wxsChoice,ArrayChoices, _("Choices"), "content", "item")
    WXS_LONG(wxsChoice,DefaultSelection, _("Selection"), "selection", -1)
}
