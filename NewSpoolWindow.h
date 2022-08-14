//
// Created by imanuel on 8/14/22.
//

#ifndef FILAMENT_MANAGER_NEWSPOOLWINDOW_H
#define FILAMENT_MANAGER_NEWSPOOLWINDOW_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "wx/spinctrl.h"
#include "wx/sizer.h"
#include "wx/valgen.h"
#include "wx/valtext.h"
#include "wx/valnum.h"

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)

enum NewSpoolWindowIDs {
    NswiApply = 200,
    NswiCancel
};

class NewSpoolWindow : public wxDialog {
public:
    explicit NewSpoolWindow(wxWindow *parent);

private:
    wxTextCtrl *name;
    wxSpinCtrlDouble *cost;
    wxSpinCtrlDouble *weight;
    wxComboBox *profile;
};


#endif //FILAMENT_MANAGER_NEWSPOOLWINDOW_H
