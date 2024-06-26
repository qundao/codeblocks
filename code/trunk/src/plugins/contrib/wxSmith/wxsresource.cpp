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
* $Revision: 13152 $
* $Id: wxsresource.cpp 13152 2023-01-14 20:35:30Z wh11204 $
* $HeadURL: svn://svn.code.sf.net/p/codeblocks/code/trunk/src/plugins/contrib/wxSmith/wxsresource.cpp $
*/

#include "wxsresource.h"
#include "wxsextresmanager.h"
#include "wxsresourcetreeitemdata.h"
#include "wxwidgets/wxsitemres.h"

#include "cbauibook.h"
#include <editormanager.h>

namespace
{
    const int EditOpenId = wxNewId();
    const int EditCloseId = wxNewId();
    const int ChangeI18NId = wxNewId();
    const int DeleteId = wxNewId();
}

// This object will cause resource to open itself
// when it's clicked in resource tree
class wxsResource::wxsResourceRootTreeItemData: public wxsResourceTreeItemData
{
    public:
        wxsResourceRootTreeItemData(wxsResource* Resource):
            wxsResourceTreeItemData(),
            m_Resource(Resource)
        {
        }

    private:

        virtual void OnSelect()
        {
            m_Resource->EditOpen();
        }

        virtual void OnRightClick()
        {
            // Build menu allowing to close this resource
            wxMenu Menu;
            if ( m_Resource->GetEditor() )
            {
                Menu.Append(EditCloseId,_("Close editor"));
            }
            else
            {
                Menu.Append(EditOpenId,_("Open editor"));
            }

            Menu.AppendSeparator();

            wxsItemRes* item = dynamic_cast <wxsItemRes *> (m_Resource);
            if (item)
            {
                Menu.Append(ChangeI18NId, item->IsI18N() ? _("Disable internationalization")
                                                         : _("Enable internationalization"));
            }

            Menu.Append(DeleteId,_("Delete this resource"));

            m_Resource->OnFillPopupMenu(&Menu);

            PopupMenu(&Menu);
        }

        virtual bool OnPopup(long Id)
        {
            if ( Id == EditOpenId )
            {
                m_Resource->EditOpen();
                return true;
            }

            if ( Id == EditCloseId )
            {
                m_Resource->EditClose();
                return true;
            }

            if ( Id == ChangeI18NId )
            {
                ToggleI18N();
                return true;
            }

            if ( Id == DeleteId )
            {
                DeleteResource();
                return true;
            }

            return m_Resource->OnPopupMenu(Id);
        }

        void ToggleI18N()
        {
            wxsItemRes* item = dynamic_cast <wxsItemRes *> (m_Resource);
            if (item)
            {
                item->SetI18N(!item->IsI18N());
            }
        }

        void DeleteResource()
        {
            if ( m_Resource->DeleteCleanup(true) )
            {
                m_Resource->GetProject()->DelResource(m_Resource);
            }
        }

        wxsResource* m_Resource;
};

wxsResource::wxsResource(wxsProject* Owner,const wxString& ResourceType,const wxString& GUI):
    m_ResourceType(ResourceType),
    m_ResourceName(wxEmptyString),
    m_GUI(GUI),
    m_Owner(Owner),
    m_Editor(nullptr),
    m_Language(wxsCPP)
{
}

wxsResource::~wxsResource()
{
    if ( m_Editor )
    {
        wxsEditor* EditorStore = m_Editor;
        m_Editor = nullptr;
        EditorStore->Close();
    }

    if ( m_TreeItemId.IsOk() )
    {
        wxsTree()->Delete(m_TreeItemId);
    }
}

void wxsResource::EditOpen()
{
    if ( m_Editor )
    {
        m_Editor->Activate();
    }
    else
    {
        wxsResourceTree::Get()->BlockSelect();
        m_Editor = OnCreateEditor(Manager::Get()->GetEditorManager()->GetNotebook());
        wxsResourceTree::Get()->UnblockSelect();
    }
}

void wxsResource::EditClose()
{
    if ( m_Editor )
    {
        m_Editor->Close();
        // Inside Close() m_Editor should be zeroed
    }
}

void wxsResource::EditorClosed()
{
    m_Editor = nullptr;
    if ( !m_Owner )
    {
        wxsExtRes()->EditorClosed(this);
    }
}

void wxsResource::BuildTreeEntry(const wxsResourceItemId& Parent)
{
    m_TreeItemId = wxsTree()->AppendItem(
        Parent,
        GetResourceName(),
        OnGetTreeIcon(), OnGetTreeIcon(),
        new wxsResourceRootTreeItemData(this));
}

bool wxsResource::ReadConfig(const TiXmlElement* Node)
{
    m_ResourceName = cbC2U(Node->Attribute("name"));
    m_Language = wxsCodeMarks::Id(cbC2U(Node->Attribute("language")));
    if ( GetResourceName().empty() )
        return false;

    return OnReadConfig(Node);
}

bool wxsResource::WriteConfig(TiXmlElement* Node)
{
    const bool Result = OnWriteConfig(Node);
    Node->SetAttribute("name",cbU2C(m_ResourceName));
    Node->SetAttribute("language",cbU2C(wxsCodeMarks::Name(m_Language)));
    return Result;
}

IMPLEMENT_CLASS(wxsResource,wxObject)
