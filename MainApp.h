//
// Created by imanuel on 27.02.22.
//

#ifndef FILAMENT_MANAGER_MAINAPP_H
#define FILAMENT_MANAGER_MAINAPP_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include "MainWindow.h"

class MainApp : public wxApp {
private:
    MainWindow *mainWindow;

public:
    MainApp();

    ~MainApp() override = default;

    bool OnInit() override;
};


#endif //FILAMENT_MANAGER_MAINAPP_H
