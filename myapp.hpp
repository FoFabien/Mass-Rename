// Un petit "Guard-Block" afin d'éviter les inclusions multiples
#ifndef MYAPP_H_INCLUDED
#define MYAPP_H_INCLUDED

// Le header wxWidgets de base
#include <wx/wx.h>

// Déclaration de notre classe dérivée de wxApp
class MyApp : public wxApp
{
    public:
        // Une seule méthode pour l'instant
        virtual bool OnInit();
};

// On n'oublie pas de déclarer cette classe comme étant la classe principale
DECLARE_APP(MyApp);

// Et on n'oublie pas également de refermer le "Guard-Block"
#endif //MYAPP_H_INCLUDED
