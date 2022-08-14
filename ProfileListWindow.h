//
// Created by imanuel on 8/13/22.
//

#ifndef FILAMENT_MANAGER_PROFILELISTWINDOW_H
#define FILAMENT_MANAGER_PROFILELISTWINDOW_H

#include <wx/wxprec.h>

#ifndef WX_PRECOMP

#include <wx/wx.h>

#endif

#include <iomanip>
#include <utility>
#include <vector>
#include <pqxx/pqxx>
#include <pqxx/result>
#include <sstream>
#include "wx/dataview.h"
#include "wx/srchctrl.h"
#include "wx/spinctrl.h"

#ifdef WXC_FROM_DIP
#undef WXC_FROM_DIP
#endif
#if wxVERSION_NUMBER >= 3100
#define WXC_FROM_DIP(x) wxWindow::FromDIP(x, NULL)
#else
#define WXC_FROM_DIP(x) x
#endif

enum FilamentProfileListColumns {
    FplColVendor = 0,
    FplColMaterial,
    FplColDiameter,
    FplColDensity,
    FplColumnCount,
};

enum ProfileListWindowActions {
    PlwaReload = 200,
    PlwaSearch,
    PlwaAddProfile,
    PlwaSave,
};

enum ProfileListWindowIDs {
    PlwiProfiles = 200,
};

class FilamentProfile {
public:
    int id;
    wxString vendor;
    wxString material;
    double density;
    double diameter;

    FilamentProfile(int id, const wxString &vendor, const wxString &material, double density, double diameter) : id(id),
                                                                                                                 vendor(vendor),
                                                                                                                 material(
                                                                                                                         material),
                                                                                                                 density(density),
                                                                                                                 diameter(
                                                                                                                         diameter) {}
};

class FilamentProfileDataViewListModel : public wxDataViewModel {
public:
    unsigned int GetChildren(const wxDataViewItem &item, wxDataViewItemArray &children) const override;

    [[nodiscard]] unsigned int GetColumnCount() const override;

    FilamentProfileDataViewListModel();

    [[nodiscard]] wxString GetColumnType(unsigned int col) const override;

    void GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const override;

    bool SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) override;

    [[nodiscard]] wxDataViewItem GetParent(const wxDataViewItem &item) const override;

    [[nodiscard]] bool IsContainer(const wxDataViewItem &item) const override;

    [[nodiscard]] int Compare(const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column,
                              bool ascending) const override;

    void Fill(const std::vector<FilamentProfile> &data);

    std::vector<FilamentProfile *> items;

    void add();
};

class DoubleCustomRenderer : public wxDataViewCustomRenderer {
public:
    DoubleCustomRenderer()
            : wxDataViewCustomRenderer("double", wxDataViewCellMode::wxDATAVIEW_CELL_EDITABLE, wxALIGN_LEFT),
              doubleValue(0) {}

    bool Render(wxRect rect, wxDC *dc, int state) override;

    bool ActivateCell(const wxRect &, wxDataViewModel *, const wxDataViewItem &, unsigned int,
                      const wxMouseEvent *mouseEvent) override;

    [[nodiscard]] wxSize GetSize() const override;

    bool SetValue(const wxVariant &value) override;

    bool GetValue(wxVariant &) const override { return true; }

    [[nodiscard]] bool HasEditorCtrl() const override { return true; }

    wxWindow *CreateEditorCtrl(wxWindow *parent, wxRect labelRect, const wxVariant &value) override;

    bool GetValueFromEditorCtrl(wxWindow *ctrl, wxVariant &value) override;

private:
    double doubleValue;
};

class ProfileListWindow : public wxFrame {
private:
    wxDataViewListCtrl *dvlFilamentProfiles;
    wxToolBar *toolBar;
    wxSearchCtrl *searchCtrl;

    FilamentProfileDataViewListModel *filamentProfilesDataViewListModel;

    void loadData(const std::string &keyword);

public:
    explicit ProfileListWindow(wxWindow *parent);

    void handleSearch(wxCommandEvent &event);

    void handleReload(wxCommandEvent &event);
};

#endif //FILAMENT_MANAGER_PROFILELISTWINDOW_H
