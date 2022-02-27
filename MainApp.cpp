//
// Created by imanuel on 27.02.22.
//

#include "MainApp.h"

MainApp::MainApp() {

}

bool MainApp::OnInit() {
    if (!wxApp::OnInit()) {
        return false;
    }

    SetAppDisplayName(_("Filament Manager"));
    mainWindow = new MainWindow();

    SetTopWindow(mainWindow);

    return GetTopWindow()->Show();
}
