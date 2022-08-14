//
// Created by imanuel on 8/13/22.
//

#include "ProfileListWindow.h"
#include "configuration.h"

unsigned int
FilamentProfileDataViewListModel::GetChildren(const wxDataViewItem &item, wxDataViewItemArray &children) const {
    for (auto elem: items) {
        children.Add(wxDataViewItem(elem));
    }

    return items.size();
}

unsigned int FilamentProfileDataViewListModel::GetColumnCount() const {
    return FplColumnCount;
}

wxString FilamentProfileDataViewListModel::GetColumnType(unsigned int col) const {
    switch (col) {
        case FplColVendor:
        case FplColMaterial:
            return "string";
        case FplColDiameter:
        case FplColDensity:
            return "double";
        default:
            wxFAIL;
            break;
    }
    return "string";
}

void
FilamentProfileDataViewListModel::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const {
    auto profile = static_cast<FilamentProfile *>(item.GetID());

    switch (col) {
        case FplColVendor:
            variant = profile->vendor;
            break;
        case FplColMaterial:
            variant = profile->material;
            break;
        case FplColDiameter:
            variant = profile->diameter;
            break;
        case FplColDensity:
            variant = profile->density;
            break;
        default:
            wxFAIL;
            break;
    }
}

bool
FilamentProfileDataViewListModel::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) {
    auto profile = static_cast<FilamentProfile *>(item.GetID());

    switch (col) {
        case FplColVendor:
            profile->vendor = variant.GetString();
            break;
        case FplColMaterial:
            profile->material = variant.GetString();
            break;
        case FplColDiameter:
            profile->diameter = variant.GetDouble();
            break;
        case FplColDensity:
            profile->density = variant.GetDouble();
            break;
        default:
            wxFAIL;
    }

    return true;
}

wxDataViewItem FilamentProfileDataViewListModel::GetParent(const wxDataViewItem &item) const {
    return wxDataViewItem(nullptr);
}

bool FilamentProfileDataViewListModel::IsContainer(const wxDataViewItem &item) const {
    return !item.IsOk();
}

void FilamentProfileDataViewListModel::Fill(const std::vector<FilamentProfile> &data) {
    items.clear();

    auto parent = wxDataViewItem(nullptr);
    for (const auto &item: data) {
        auto profile = new FilamentProfile(item.id, item.vendor, item.material, item.density, item.diameter);
        items.emplace_back(profile);
        ItemAdded(parent, wxDataViewItem(profile));
    }
}

FilamentProfileDataViewListModel::FilamentProfileDataViewListModel()
        : wxDataViewModel(), items(std::vector<FilamentProfile *>()) {}

int
FilamentProfileDataViewListModel::Compare(const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column,
                                          bool ascending) const {
    auto item1Data = static_cast<FilamentProfile *>(item1.GetID());
    auto item2Data = static_cast<FilamentProfile *>(item2.GetID());

    switch (column) {
        case FplColVendor:
        case FplColMaterial:
            return wxDataViewModel::Compare(item1, item2, column, ascending);
        case FplColDiameter:
        case FplColDensity:
            if (item1Data->diameter == item2Data->diameter) {
                return 0;
            } else if (item1Data->diameter < item2Data->diameter) {
                return ascending ? 1 : -1;
            } else if (item1Data->diameter > item2Data->diameter) {
                return ascending ? -1 : 1;
            }
            break;
    }

    return 0;
}

void FilamentProfileDataViewListModel::add() {
    auto profile = new FilamentProfile(-1, "", "", 1.24, 1.75);
    items.emplace_back(profile);
    ItemAdded(wxDataViewItem(nullptr), wxDataViewItem(profile));
}

void ProfileListWindow::loadData(const std::string &keyword) {
    try {
        pqxx::connection connection{CONNECTION_STRING};
        pqxx::work txn{connection};
        auto query =
                "SELECT profile.id, profile.vendor, profile.material, profile.diameter, profile.density FROM profiles profile WHERE profile.vendor ILIKE '%" +
                txn.esc(keyword) + "%' OR profile.vendor ILIKE '%" + txn.esc(keyword) +
                "%' ORDER BY profile.vendor, profile.material";

        pqxx::result resultSet{txn.exec(query)};

        auto items = std::vector<FilamentProfile>();

        for (auto row: resultSet) {
            items.emplace_back(FilamentProfile(row["id"].as<int>(), wxString::FromUTF8(row["vendor"].c_str()),
                                               wxString::FromUTF8(row["material"].c_str()), row["density"].as<double>(),
                                               row["diameter"].as<double>()));
        }

        txn.commit();

        filamentProfilesDataViewListModel->Fill(items);
        dvlFilamentProfiles->AssociateModel(filamentProfilesDataViewListModel);
    } catch (std::exception &exception) {
        wxLogStatus(exception.what());
    }
}

ProfileListWindow::ProfileListWindow(wxWindow *parent) : wxFrame(parent, wxID_ANY, _("Profile"), wxDefaultPosition,
                                                                 wxSize(640, 480),
                                                                 wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    SetMinClientSize(wxSize(640, 480));
    filamentProfilesDataViewListModel = new FilamentProfileDataViewListModel();

    toolBar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_NOICONS | wxTB_TEXT);
    toolBar->AddTool(PlwaReload, _("Profile laden"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, "", "", nullptr);
    toolBar->AddTool(PlwaAddProfile, _(L"Profil hinzufügen"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, "", "",
                     nullptr);
    toolBar->AddTool(PlwaSave, _(L"Profile speichern"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, "", "", nullptr);

    searchCtrl = new wxSearchCtrl(toolBar, PlwaSearch, wxEmptyString, wxDefaultPosition, wxSize(200, -1),
                                  wxTE_PROCESS_ENTER);
    searchCtrl->ShowSearchButton(true);
    toolBar->AddControl(searchCtrl);
    toolBar->Realize();
    CreateStatusBar();

    auto panel = new wxPanel(this);
    auto contentSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(contentSizer);

    dvlFilamentProfiles = new wxDataViewListCtrl(panel, PlwiProfiles, wxDefaultPosition,
                                                 wxDLG_UNIT(this, wxSize(-1, -1)), wxDV_ROW_LINES | wxDV_SINGLE);
    dvlFilamentProfiles->AppendTextColumn(_("Hersteller"), wxDATAVIEW_CELL_EDITABLE, WXC_FROM_DIP(-2),
                                          wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                        wxDATAVIEW_COL_SORTABLE);
    dvlFilamentProfiles->AppendTextColumn(_("Material"), wxDATAVIEW_CELL_EDITABLE, WXC_FROM_DIP(-2),
                                          wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                        wxDATAVIEW_COL_SORTABLE);
    dvlFilamentProfiles->AppendColumn(
            new wxDataViewColumn(_("Durchmesser (mm)"), new DoubleCustomRenderer(), FplColDiameter, WXC_FROM_DIP(-2),
                                 wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                 wxDATAVIEW_COL_SORTABLE), "double");
    dvlFilamentProfiles->AppendColumn(
            new wxDataViewColumn(_(L"Dichte (g/cm³)"), new DoubleCustomRenderer(), FplColDensity, WXC_FROM_DIP(-2),
                                 wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                 wxDATAVIEW_COL_SORTABLE), "double");

    contentSizer->Add(dvlFilamentProfiles, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    SetName("ProfileListWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if (GetSizer()) {
        GetSizer()->Fit(panel);
    }
    if (GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    Bind(wxEVT_MENU, &ProfileListWindow::handleReload, this, PlwaReload);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &ProfileListWindow::handleSearch, this, PlwaSearch);
    Bind(wxEVT_TEXT_ENTER, &ProfileListWindow::handleSearch, this, PlwaSearch);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        filamentProfilesDataViewListModel->add();
    }, PlwaAddProfile);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        pqxx::connection connection{CONNECTION_STRING};
        pqxx::work txn{connection};
        for (const auto profile: filamentProfilesDataViewListModel->items) {
            try {
                if (profile->id == -1) {
                    auto query = "INSERT INTO profiles (vendor, material, density, diameter) VALUES ('" +
                                 txn.esc(profile->vendor.ToStdString()) + "', '" +
                                 txn.esc(profile->material.ToStdString()) + "', " +
                                 std::to_string(profile->density) + ", " +
                                 std::to_string(profile->diameter) + ")";
                    txn.exec(query);
                } else {
                    auto query = "UPDATE profiles SET vendor = '" + txn.esc(profile->vendor.ToStdString())
                                 + "', material = '" + txn.esc(profile->material.ToStdString()) + "'"
                                 + ", density = " + std::to_string(profile->density) + ""
                                 + ", diameter = " + std::to_string(profile->diameter) + " "
                                 + "WHERE id = " + std::to_string(profile->id);
                    txn.exec(query);
                }
            } catch (std::exception &exception) {
                wxLogStatus(exception.what());
                txn.abort();
            }
        }
        txn.commit();
    }, PlwaSave);
    loadData("");
}

void ProfileListWindow::handleSearch(wxCommandEvent &event) {
    loadData(event.GetString().utf8_str().data());
}

void ProfileListWindow::handleReload(wxCommandEvent &event) {
    searchCtrl->SetValue(wxEmptyString);
    loadData("");
}

bool DoubleCustomRenderer::Render(wxRect rect, wxDC *dc, int state) {
    dc->SetPen(*wxTRANSPARENT_PEN);
    rect.Deflate(2);

    RenderText(std::to_string(doubleValue), 0, rect, dc, state);
    return true;
}

bool DoubleCustomRenderer::ActivateCell(const wxRect &, wxDataViewModel *, const wxDataViewItem &, unsigned int,
                                        const wxMouseEvent *mouseEvent) {
    return false;
}

wxSize DoubleCustomRenderer::GetSize() const {
    return GetView()->FromDIP(wxSize(60, 20));
}

bool DoubleCustomRenderer::SetValue(const wxVariant &value) {
    doubleValue = value.GetDouble();

    return true;
}

wxWindow *DoubleCustomRenderer::CreateEditorCtrl(wxWindow *parent, wxRect labelRect, const wxVariant &value) {
    auto ctrl = new wxSpinCtrlDouble(parent, wxID_ANY, value, labelRect.GetPosition(), labelRect.GetSize(),
                                     wxSP_ARROW_KEYS | wxTE_PROCESS_ENTER, 0, 1000, value, 0.01);
    ctrl->SetValue(value.GetDouble());

    return ctrl;
}

bool DoubleCustomRenderer::GetValueFromEditorCtrl(wxWindow *ctrl, wxVariant &value) {
    auto spinCtrl = dynamic_cast<wxSpinCtrlDouble *>(ctrl);
    if (!spinCtrl)
        return false;

    value = spinCtrl->GetValue();

    return true;
}
