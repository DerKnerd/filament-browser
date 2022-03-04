//
// Created by imanuel on 27.02.22.
//

#include "MainWindow.h"

unsigned int
FilamentSpoolDataViewListModel::GetChildren(const wxDataViewItem &item, wxDataViewItemArray &children) const {
    for (auto elem: items) {
        children.Add(wxDataViewItem(elem));
    }

    return items.size();
}

unsigned int FilamentSpoolDataViewListModel::GetColumnCount() const {
    return SpoolColumnCount;
}

wxString FilamentSpoolDataViewListModel::GetColumnType(unsigned int col) const {
    return "string";
}

void FilamentSpoolDataViewListModel::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const {
    auto spool = (FilamentSpool *) item.GetID();

    switch (col) {
        case ColName:
            variant = spool->name;
            break;
        case ColVendor:
            variant = spool->vendor;
            break;
        case ColMaterial:
            variant = spool->material;
            break;
        case ColWeightAvailable:
            variant = spool->getWeightAvailable();
            break;
        case ColWeightInitial:
            variant = spool->getWeightInitial();
            break;
        case ColDiameter:
            variant = spool->getDiameter();
            break;
        default:
            wxFAIL;
            break;
    }
}

bool FilamentSpoolDataViewListModel::SetValue(const wxVariant &variant, const wxDataViewItem &item, unsigned int col) {
    return true;
}

wxDataViewItem FilamentSpoolDataViewListModel::GetParent(const wxDataViewItem &item) const {
    return wxDataViewItem(nullptr);
}

bool FilamentSpoolDataViewListModel::IsContainer(const wxDataViewItem &item) const {
    return !item.IsOk();
}

void FilamentSpoolDataViewListModel::Fill(const std::vector<FilamentSpool> &data) {
    items.clear();

    auto parent = wxDataViewItem(nullptr);
    for (const auto &item: data) {
        auto spool = new FilamentSpool(item.name, item.vendor, item.material, item.weightUsed, item.weightInitial,
                                       item.diameter);
        items.emplace_back(spool);
        ItemAdded(parent, wxDataViewItem(spool));
    }
}

FilamentSpoolDataViewListModel::FilamentSpoolDataViewListModel()
        : wxDataViewModel(), items(std::vector<FilamentSpool *>()) {}

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, _("Filament Manager"), wxDefaultPosition, wxSize(1280, 600),
                                   wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    SetMinClientSize(wxSize(1280, 600));
    filamentSpoolDataViewListModel = new FilamentSpoolDataViewListModel();

    toolBar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_NOICONS | wxTB_TEXT);
    toolBar->AddTool(MainWindowActions::Reload, _("Filamente laden"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, "", "",
                     nullptr);
    toolBar->AddTool(MainWindowActions::CopyName, _("Filamentname kopieren"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL,
                     "", "", nullptr);

    searchCtrl = new wxSearchCtrl(toolBar, Search, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
    searchCtrl->ShowSearchButton(true);
    toolBar->AddControl(searchCtrl);
    toolBar->Realize();
    CreateStatusBar();

    auto panel = new wxPanel(this);
    auto contentSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(contentSizer);

    dvlFilamentSpools = new wxDataViewListCtrl(panel, Spools, wxDefaultPosition,
                                               wxDLG_UNIT(this, wxSize(-1, -1)), wxDV_ROW_LINES | wxDV_SINGLE);
    dvlFilamentSpools->AppendTextColumn(_("Name"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
    dvlFilamentSpools->AppendTextColumn(_("Hersteller"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
    dvlFilamentSpools->AppendTextColumn(_("Material"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
    dvlFilamentSpools->AppendTextColumn(_("Restgewicht"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
    dvlFilamentSpools->AppendTextColumn(_(L"Ursprüngliches Gewicht"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);
    dvlFilamentSpools->AppendTextColumn(_("Durchmesser"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_RIGHT,
                                        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE);

    contentSizer->Add(dvlFilamentSpools, 1, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    SetName("MainWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if (GetSizer()) {
        GetSizer()->Fit(panel);
    }
    if (GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    Bind(wxEVT_MENU, &MainWindow::handleReload, this, Reload);
    Bind(wxEVT_MENU, &MainWindow::handleCopyName, this, CopyName);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &MainWindow::handleSearch, this, Search);
    Bind(wxEVT_TEXT_ENTER, &MainWindow::handleSearch, this, Search);
    loadData("");
}

void MainWindow::handleReload(wxCommandEvent &event) {
    searchCtrl->SetValue(wxEmptyString);
    loadData("");
}

void MainWindow::handleCopyName(wxCommandEvent &event) {
    if (dvlFilamentSpools->HasSelection()) {
        auto selectedItem = dvlFilamentSpools->GetSelection();
        auto spool = (FilamentSpool *) selectedItem.GetID();
        if (wxClipboard::Get()->Open()) {
            wxClipboard::Get()->SetData(new wxTextDataObject(spool->name));
            wxClipboard::Get()->Close();
            wxLogStatus(spool->name + " kopiert");
        }
    }
}

void MainWindow::handleSearch(wxCommandEvent &event) {
    loadData(event.GetString().utf8_str().data());
}

void MainWindow::loadData(std::string keyword) {
    try {
        pqxx::connection connection{CONNECTION_STRING};
        pqxx::work txn{connection};
        auto query =
                "SELECT profile.vendor, profile.material, profile.diameter, spool.name, spool.used, spool.weight FROM spools spool JOIN profiles profile on profile.id = spool.profile_id WHERE spool.used <> spool.weight AND spool.name LIKE '%" +
                txn.esc(keyword) + "%' ORDER BY profile.vendor, profile.material, spool.name";

        pqxx::result resultSet{txn.exec(query)};

        auto items = std::vector<FilamentSpool>();

        for (auto row: resultSet) {
            items.emplace_back(FilamentSpool(
                    wxString::FromUTF8(row["name"].c_str()), wxString::FromUTF8(row["vendor"].c_str()),
                    wxString::FromUTF8(row["material"].c_str()), row["used"].as<double>(), row["weight"].as<double>(),
                    row["diameter"].as<float>()));
        }

        txn.commit();

        filamentSpoolDataViewListModel->Fill(items);
        dvlFilamentSpools->AssociateModel(filamentSpoolDataViewListModel);
    } catch (std::exception &exception) {
        wxLogStatus(exception.what());
    }
}
