#ifndef MAINFRAME_H
#define MAINFRAME_H

// Le header wxWidgets de base
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/statline.h>
#include <wx/clrpicker.h>

enum
{
    ID_BTN_RENAME,
    ID_BTN_HELP
};

class MainFrame : public wxFrame
{
    public:
        MainFrame(const wxString& title);
        virtual ~MainFrame();
        void OnDropFiles(const wxArrayString& filenames);
    private:
        wxButton *m_rename, *m_help;
        wxTextCtrl *m_output, *m_list, *m_start;

        // events
        void OnBtnRename(wxCommandEvent &event);
        void OnBtnHelp(wxCommandEvent &event);

        // event table
        DECLARE_EVENT_TABLE()
};

#endif // MAINFRAME_H
