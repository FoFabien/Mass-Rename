#include "myapp.hpp"
#include "mainframe.hpp"
#include "version.h"

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    std::string name = "Mass Rename " + std::string(AutoVersion::STATUS) + " v" + std::string(AutoVersion::FULLVERSION_STRING);

    MainFrame *frm=new MainFrame(wxString(name.c_str(), wxConvUTF8));
    frm->Show();
    return true;
}
