/** \file wxscombobox.cpp
*
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
* $Id: wxscombobox.cpp 13235 2023-03-15 14:00:15Z wh11204 $
* $HeadURL: svn://svn.code.sf.net/p/codeblocks/code/trunk/src/plugins/contrib/wxSmith/wxwidgets/defitems/wxscombobox.cpp $
*/

#include <wx/combobox.h>
#include "wxscombobox.h"

#include <prep.h>

namespace
{
    wxsRegisterItem<wxsComboBox> Reg(_T("ComboBox"),wxsTWidget,_T("Standard"),290);


    WXS_ST_BEGIN(wxsComboBoxStyles,_T(""))
        WXS_ST_CATEGORY("wxComboBox")
        WXS_ST(wxCB_SIMPLE)
        WXS_ST(wxCB_SORT)
        WXS_ST(wxCB_READONLY)
        WXS_ST(wxCB_DROPDOWN)
        WXS_ST(wxTE_PROCESS_ENTER)
        WXS_ST_DEFAULTS()
    WXS_ST_END()

    WXS_EV_BEGIN(wxsComboBoxEvents)
        WXS_EVI(EVT_COMBOBOX,wxEVT_COMMAND_COMBOBOX_SELECTED,wxCommandEvent,Selected)
        WXS_EVI(EVT_COMBOBOX_DROPDOWN,wxEVT_COMMAND_COMBOBOX_DROPDOWN,wxCommandEvent,Dropdown)  // added in 3.0
        WXS_EVI(EVT_COMBOBOX_CLOSEUP,wxEVT_COMMAND_COMBOBOX_CLOSEUP,wxCommandEvent,CloseUp)     // added in 3.0
        WXS_EVI(EVT_TEXT,wxEVT_COMMAND_TEXT_UPDATED,wxCommandEvent,TextUpdated)
        WXS_EVI(EVT_TEXT_ENTER,wxEVT_COMMAND_TEXT_ENTER,wxCommandEvent,TextEnter)
    WXS_EV_END()
}

wxsComboBox::wxsComboBox(wxsItemResData* Data):
    wxsWidget(
        Data,
        &Reg.Info,
        wxsComboBoxEvents,
        wxsComboBoxStyles),
    DefaultSelection(-1)
{}

void wxsComboBox::OnBuildCreatingCode()
{
    switch ( GetLanguage() )
    {
        case wxsCPP:
        {
            AddHeader(_T("<wx/combobox.h>"),GetInfo().ClassName,hfInPCH);
            Codef(_T("%C(%W, %I, wxEmptyString, %P, %S, 0, 0, %T, %V, %N);\n"));

            for ( size_t i = 0; i <  ArrayChoices.GetCount(); ++i )
            {
                if ( DefaultSelection == (int)i )
                {
                    Codef(_T("%ASetSelection( "));;
                }
                Codef( _T("%AAppend(%t)"), ArrayChoices[i].wx_str());
                if ( DefaultSelection == (int)i )
                {
                    Codef(_T(" )"));
                }
                Codef(_T(";\n"));
            }

            BuildSetupWindowCode();
            return;
        }

        case wxsUnknownLanguage: // fall-through
        default:
        {
            wxsCodeMarks::Unknown(_T("wxsComboBox::OnBuildCreatingCode"),GetLanguage());
        }
    }
}

wxObject* wxsComboBox::OnBuildPreview(wxWindow* Parent,long Flags)
{
    wxComboBox* Preview = new wxComboBox(Parent,GetId(),_T(""),Pos(Parent),Size(Parent),ArrayChoices, Style());

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

void wxsComboBox::OnEnumWidgetProperties(cb_unused long Flags)
{
    WXS_ARRAYSTRING(wxsComboBox,ArrayChoices, _("Choices"), "content", "item")
    WXS_LONG(wxsComboBox,DefaultSelection, _("Selection"), "selection", -1)
}
