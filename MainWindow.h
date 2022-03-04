//
// Created by imanuel on 27.02.22.
//

#ifndef FILAMENT_MANAGER_MAINWINDOW_H
#define FILAMENT_MANAGER_MAINWINDOW_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <wx/clipbrd.h>
#include <wx/dataview.h>
#include <wx/persist.h>
#include <wx/srchctrl.h>
#include <utility>
#include <vector>
#include <sstream>
#include <iomanip>
#include <pqxx/pqxx>
#include "configuration.h"

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

enum FilamentSpoolListColumns {
    ColName = 0,
    ColVendor,
    ColMaterial,
    ColWeightAvailable,
    ColWeightInitial,
    ColDiameter,
    SpoolColumnCount,
};

enum MainWindowActions {
    Reload = 200,
    CopyName,
    Search,
};

enum MainWindowIDs {
    Spools = 200,
};

class FilamentSpool {
public:
    FilamentSpool(const wxString &name, const wxString &vendor, const wxString &material, double weightUsed,
                  double weightInitial, float diameter) : name(name), vendor(vendor), material(material),
                                                          weightUsed(weightUsed), weightInitial(weightInitial),
                                                          diameter(diameter) {}

    wxString name;
    wxString vendor;
    wxString material;
    double weightUsed;
    double weightInitial;
    float diameter;

    [[nodiscard]] wxString getWeightAvailable() const {
        auto ss = std::stringstream();
        ss << std::fixed << std::setprecision(2) << weightInitial - weightUsed << "g";

        return ss.str();
    }

    [[nodiscard]] wxString getWeightInitial() const {
        auto ss = std::stringstream();
        ss << std::fixed << std::setprecision(2) << weightInitial << "g";

        return ss.str();
    }

    [[nodiscard]] wxString getDiameter() const {
        auto ss = std::stringstream();
        ss << std::fixed << std::setprecision(2) << diameter << "mm";

        return ss.str();
    }
};

class FilamentSpoolDataViewListModel : public wxDataViewModel {
public:
    unsigned int GetChildren(const wxDataViewItem &item, wxDataViewItemArray &children) const override;

    [[nodiscard]] unsigned int GetColumnCount() const override;

    FilamentSpoolDataViewListModel();

    [[nodiscard]] wxString GetColumnType(unsigned int col) const override;

    void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const override;

    bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) override;

    [[nodiscard]] wxDataViewItem GetParent(const wxDataViewItem &item) const override;

    [[nodiscard]] bool IsContainer(const wxDataViewItem &item) const override;

    void Fill(const std::vector<FilamentSpool> &data);

    std::vector<FilamentSpool *> items;
};

class MainWindow : public wxFrame {
private:
    wxDataViewListCtrl *dvlFilamentSpools;
    wxToolBar *toolBar;
    wxSearchCtrl *searchCtrl;

    FilamentSpoolDataViewListModel *filamentSpoolDataViewListModel;

    void loadData(std::string keyword);
public:
    MainWindow();

    void handleSearch(wxCommandEvent& event);

    void handleReload(wxCommandEvent &event);

    void handleCopyName(wxCommandEvent &event);
};


#endif //FILAMENT_MANAGER_MAINWINDOW_H
