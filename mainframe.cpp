#include "mainframe.hpp"

#include <wx/dnd.h>

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_BUTTON(ID_BTN_RENAME, MainFrame::OnBtnRename)
    EVT_BUTTON(ID_BTN_HELP, MainFrame::OnBtnHelp)
END_EVENT_TABLE()

wxArrayString& split(const wxString &s, wxChar delim, wxArrayString &elems)
{
    wxString item;

    for(size_t i = 0; i < s.size(); ++i)
    {
        if(s[i] == delim)
        {
            if(!item.IsEmpty()) elems.Add(item);
            item.Clear();
        }
        else item += s[i];
    }
    if(!item.IsEmpty()) elems.Add(item);
    return elems;
}

wxString replace(const wxString& s, const wxChar& c, const wxString& r)
{
    wxString result;
    for(size_t i = 0; i < s.size(); ++i)
    {
        if(s[i] == c) result.Append(r);
        else result.Append(s[i]);
    }
    return result;
}

wxArrayString separatePath(const wxString& s)
{
    wxArrayString result;
    result.Add(wxString());
    result.Add(wxString());
    int pos = -1;

    for(int i = 0; i < s.size(); ++i)
        if(s[i] == '\\') pos = i;

    if(pos == -1)
    {
        result[1] = s;
    }
    else
    {
        result[0] = s.SubString(0, pos);
        result[1] = s.SubString(pos+1, s.size());
    }
    return result;
}

wxString getNumber(int current, int max)
{
    int a = 0;
    int b = 0;
    int c = current;
    while(max >= 10)
    {
        a++;
        max /= 10;
    }
    while(c >= 10)
    {
        b++;
        c /= 10;
    }
    wxString result = wxString::Format(wxT("%i"),current);

    for(int i = 0 ; i < a - b; ++i)
        result.insert(result.begin(), '0');
    return result;
}

class DnDFile : public wxFileDropTarget
{
    public:
        DnDFile(MainFrame *pOwner = NULL) { m_pOwner = pOwner; }

        virtual bool OnDropFiles(wxCoord x, wxCoord y,const wxArrayString& filenames)
        {
            if(filenames.size() > 0)
            {
                m_pOwner->OnDropFiles(filenames);
                return true;
            }
            return false;
        };

    private:
        MainFrame *m_pOwner;
};

MainFrame::MainFrame(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(500, 500), wxDEFAULT_FRAME_STYLE)
{
    wxFrame::SetIcon(wxIcon(_T("MAINICON")));

    SetMinSize(wxSize(500, 500));

    wxPanel *panel = new wxPanel(this, -1);

    wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *hbox4 = new wxBoxSizer(wxHORIZONTAL);

    vbox->Add(hbox1, 0, wxEXPAND);
    vbox->Add(hbox2, 0, wxALIGN_LEFT);
    vbox->Add(hbox3, 0, wxALIGN_LEFT | wxEXPAND);
    vbox->Add(hbox4, 0, wxEXPAND);

    m_rename = new wxButton(panel, ID_BTN_RENAME, _T("Rename with :"));
    hbox1->Add(m_rename);
    m_output = new wxTextCtrl(panel, -1, _T(""));
    hbox1->Add(m_output, 20);
    m_help = new wxButton(panel, ID_BTN_HELP, _T("?"));
    hbox1->Add(m_help, 1);

    hbox2->Add(new wxStaticText(panel, -1, _T("Start from number :")));
    m_start = new wxTextCtrl(panel, -1, _T("1"));
    hbox2->Add(m_start);

    hbox3->Add(new wxStaticLine(panel, -1, wxPoint(0, 5), wxSize(1, 1)), 1);

    hbox4->Add(new wxStaticText(panel, -1, _T("File list")));

    m_list = new wxTextCtrl(panel, -1, _T(""), wxPoint(0, 0), wxSize(0, 0), wxTE_MULTILINE);
    vbox->Add(m_list, 1, wxEXPAND);

    panel->SetSizer(vbox);

    this->SetDropTarget((wxDropTarget*)new DnDFile(this));
}

MainFrame::~MainFrame()
{

}

void MainFrame::OnDropFiles(const wxArrayString& filenames)
{
    for(size_t i = 0; i < filenames.size(); ++i)
    {
        if((i == 0 && !m_list->GetValue().IsEmpty()) || i != 0) m_list->AppendText(wxString(_T("\n")));
        m_list->AppendText(filenames[i]);
    }

}

void MainFrame::OnBtnRename(wxCommandEvent &event)
{
    wxString out = m_output->GetValue();
    if(out.IsEmpty())
    {
        wxMessageBox(_T("Please specify the output name"));
        return;
    }

    int joker = 0;
    for(size_t i = 0; i < out.size(); ++i)
    {
        if(out[i] == '*') joker++;
    }

    if(joker == 0)
    {
        wxMessageBox(_T("Joker '*' is missing in the output name"));
        return;
    }

    long lstart;
    if(!m_start->GetValue().ToLong(&lstart))
    {
        wxMessageBox(_T("Invalid starting number"));
        return;
    }
    int istart = (int)lstart;
    if(istart < 0)
    {
        wxMessageBox(_T("Starting number must be positive"));
        return;
    }

    joker = 0; // will be used to count errors

    wxArrayString files;
    split(m_list->GetValue(), '\n', files);
    wxLogNull *log = new wxLogNull();
    // remove empty string
    for(size_t i = 0; i < files.Count(); ++i)
    {
        if(files[i].IsEmpty())
        {
            files.erase(files.begin()+i);
            i--;
        }
    }

    // security
    for(size_t i = 0; i < files.Count(); ++i)
    {
        wxArrayString f = separatePath(files[i]);
        wxString newname = replace(_T("renamer_temporary_file_security_*"), '*', getNumber(i+lstart, files.Count()+lstart));
        if(!wxRenameFile(f[0] + f[1], f[0] + newname, true))
        {
            joker++;
            files[i].Clear();
        }
        else files[i] = f[0] + newname;
    }

    for(size_t i = 0; i < files.Count(); ++i)
    {
        if(!files[i].IsEmpty())
        {
            wxArrayString f = separatePath(files[i]);
            wxString newname = replace(out, '*', getNumber(i+lstart, files.Count()+lstart));
            if(!wxRenameFile(f[0] + f[1], f[0] + newname, true)) joker++;
        }
    }
    delete log;
    wxMessageBox(_T("Done\n") + wxString::Format(wxT("%i"),files.Count()) + _T(" file(s)\n") + wxString::Format(wxT("%i"),joker) + _T(" error(s)\n"));
}

void MainFrame::OnBtnHelp(wxCommandEvent &event)
{
    wxMessageBox(_T("Usage\n- Add your file in the order you want them renamed.\n\n- Set the output name. It MUST contain the '*' character.\nExample :\t\"example*.txt\"\nResult :\t\"example1.txt\"\n\t\"example2.txt\"\n\tetc...\n\n- Press the rename button"), _T("Help"), wxICON_QUESTION);
}
