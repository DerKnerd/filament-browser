//
// Created by imanuel on 27.02.22.
//

#include "MainWindow.h"
#include "ProfileListWindow.h"
#include "NewSpoolWindow.h"

unsigned int
FilamentSpoolDataViewListModel::GetChildren(const wxDataViewItem &item, wxDataViewItemArray &children) const {
    for (auto elem: items) {
        children.Add(wxDataViewItem(elem));
    }

    return items.size();
}

unsigned int FilamentSpoolDataViewListModel::GetColumnCount() const {
    return FslColumnCount;
}

wxString FilamentSpoolDataViewListModel::GetColumnType(unsigned int col) const {
    return "string";
}

void FilamentSpoolDataViewListModel::GetValue(wxVariant &variant, const wxDataViewItem &item, unsigned int col) const {
    auto spool = (FilamentSpool *) item.GetID();

    switch (col) {
        case FslColName:
            variant = spool->name;
            break;
        case FslColVendor:
            variant = spool->vendor;
            break;
        case FslColMaterial:
            variant = spool->material;
            break;
        case FslColWeightAvailable:
            variant = spool->getWeightAvailable();
            break;
        case FslColWeightInitial:
            variant = spool->getWeightInitial();
            break;
        case FslColDiameter:
            variant = spool->getDiameter();
            break;
        case FslColPricePerKilo:
            variant = spool->getPricePerKilo();
            break;
        case FslColPricePerSpool:
            variant = spool->getPricePerSpool();
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
                                       item.diameter, item.cost);
        items.emplace_back(spool);
        ItemAdded(parent, wxDataViewItem(spool));
    }
}

FilamentSpoolDataViewListModel::FilamentSpoolDataViewListModel()
        : wxDataViewModel(), items(std::vector<FilamentSpool *>()) {}

int
FilamentSpoolDataViewListModel::Compare(const wxDataViewItem &item1, const wxDataViewItem &item2, unsigned int column,
                                        bool ascending) const {
    auto item1Data = static_cast<FilamentSpool *>(item1.GetID());
    auto item2Data = static_cast<FilamentSpool *>(item2.GetID());

    auto weightAvailable1 = item1Data->weightInitial - item1Data->weightUsed;
    auto weightAvailable2 = item2Data->weightInitial - item2Data->weightUsed;
    auto pricePerKilo1 = (item1Data->cost / item1Data->weightInitial * 1000);
    auto pricePerKilo2 = (item2Data->cost / item2Data->weightInitial * 1000);
    switch (column) {
        case FslColName:
        case FslColVendor:
        case FslColMaterial:
            return wxDataViewModel::Compare(item1, item2, column, ascending);
        case FslColWeightAvailable:
            if (weightAvailable1 == weightAvailable2) {
                return 0;
            } else if (weightAvailable1 < weightAvailable2) {
                return ascending ? 1 : -1;
            } else if (weightAvailable1 > weightAvailable2) {
                return ascending ? -1 : 1;
            }
            break;
        case FslColWeightInitial:
            if (item1Data->weightInitial == item2Data->weightInitial) {
                return 0;
            } else if (item1Data->weightInitial < item2Data->weightInitial) {
                return ascending ? 1 : -1;
            } else if (item1Data->weightInitial > item2Data->weightInitial) {
                return ascending ? -1 : 1;
            }
            break;
        case FslColDiameter:
            if (item1Data->diameter == item2Data->diameter) {
                return 0;
            } else if (item1Data->diameter < item2Data->diameter) {
                return ascending ? 1 : -1;
            } else if (item1Data->diameter > item2Data->diameter) {
                return ascending ? -1 : 1;
            }
            break;
        case FslColPricePerKilo:
            if (pricePerKilo1 == weightAvailable2) {
                return 0;
            } else if (pricePerKilo1 < pricePerKilo2) {
                return ascending ? 1 : -1;
            } else if (pricePerKilo1 > pricePerKilo2) {
                return ascending ? -1 : 1;
            }
            break;
        case FslColPricePerSpool:
            if (item1Data->cost == item2Data->cost) {
                return 0;
            } else if (item1Data->cost < item2Data->cost) {
                return ascending ? 1 : -1;
            } else if (item1Data->cost > item2Data->cost) {
                return ascending ? -1 : 1;
            }
            break;
    }

    return 0;
}

MainWindow::MainWindow() : wxFrame(nullptr, wxID_ANY, _("Filament Manager"), wxDefaultPosition, wxSize(1280, 600),
                                   wxDEFAULT_FRAME_STYLE | wxCLIP_CHILDREN) {
    SetMinClientSize(wxSize(1280, 600));
    filamentSpoolDataViewListModel = new FilamentSpoolDataViewListModel();

    toolBar = CreateToolBar(wxTB_HORIZONTAL | wxTB_HORZ_LAYOUT | wxTB_NOICONS | wxTB_TEXT);
    toolBar->AddTool(MainWindowActions::MwaReload, _("Filamente laden"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, "",
                     "", nullptr);
    toolBar->AddTool(MainWindowActions::MwaNewSpool, _(L"Neue Filamentrolle"), wxNullBitmap, wxNullBitmap,
                     wxITEM_NORMAL, "", "", nullptr);
    toolBar->AddTool(MainWindowActions::MwaCopyName, _("Filamentname kopieren"), wxNullBitmap, wxNullBitmap,
                     wxITEM_NORMAL, "", "", nullptr);
    toolBar->AddSeparator();
    toolBar->AddTool(MainWindowActions::MwaOpenProfiles, _(L"Profile öffnen"), wxNullBitmap, wxNullBitmap,
                     wxITEM_NORMAL, "", "", nullptr);

    searchCtrl = new wxSearchCtrl(toolBar, MwaSearch, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxALIGN_RIGHT |
                                                                                                         wxTE_PROCESS_ENTER);
    searchCtrl->ShowSearchButton(true);
    toolBar->AddControl(searchCtrl);
    toolBar->Realize();
    CreateStatusBar();

    auto panel = new wxPanel(this);
    auto contentSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(contentSizer);

    dvlFilamentSpools = new wxDataViewListCtrl(panel, MwiSpools, wxDefaultPosition,
                                               wxDLG_UNIT(this, wxSize(-1, -1)), wxDV_ROW_LINES | wxDV_SINGLE);
    dvlFilamentSpools->AppendTextColumn(_("Name"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_LEFT,
                                        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                        wxDATAVIEW_COL_SORTABLE);
    dvlFilamentSpools->AppendTextColumn(_("Hersteller"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                      wxDATAVIEW_COL_SORTABLE);
    dvlFilamentSpools->AppendTextColumn(_("Material"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_LEFT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                      wxDATAVIEW_COL_SORTABLE);
    dvlFilamentSpools->AppendTextColumn(_("Restgewicht"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                       wxDATAVIEW_COL_SORTABLE);
    dvlFilamentSpools->AppendTextColumn(wxString::FromUTF8("Ursprüngliches Gewicht"), wxDATAVIEW_CELL_INERT,
                                        WXC_FROM_DIP(-2),
                                        wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                       wxDATAVIEW_COL_SORTABLE);
    dvlFilamentSpools->AppendTextColumn(_(L"Preis pro Kilo"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                       wxDATAVIEW_COL_SORTABLE);
    dvlFilamentSpools->AppendTextColumn(_(L"Preis pro Rolle"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2),
                                        wxALIGN_RIGHT, wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                                       wxDATAVIEW_COL_SORTABLE);
    dvlFilamentSpools->AppendTextColumn(_("Durchmesser"), wxDATAVIEW_CELL_INERT, WXC_FROM_DIP(-2), wxALIGN_RIGHT,
                                        wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE |
                                        wxDATAVIEW_COL_SORTABLE);

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

    Bind(wxEVT_MENU, &MainWindow::handleReload, this, MwaReload);
    Bind(wxEVT_MENU, &MainWindow::handleCopyName, this, MwaCopyName);
    Bind(wxEVT_SEARCHCTRL_SEARCH_BTN, &MainWindow::handleSearch, this, MwaSearch);
    Bind(wxEVT_TEXT_ENTER, &MainWindow::handleSearch, this, MwaSearch);
    Bind(wxEVT_MENU, [this](wxCommandEvent &) {
        auto window = new ProfileListWindow(this);
        window->Show();
    }, MwaOpenProfiles);
    Bind(wxEVT_MENU, [this](wxCommandEvent &event) {
        auto window = new NewSpoolWindow(this);
        window->ShowModal();

        handleSearch(event);
    }, MwaNewSpool);
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

void MainWindow::loadData(const std::string &keyword) {
    try {
        pqxx::connection connection{CONNECTION_STRING};
        pqxx::work txn{connection};
        auto query =
                "SELECT profile.vendor, profile.material, profile.diameter, spool.name, spool.used, spool.weight, spool.cost FROM spools spool JOIN profiles profile on profile.id = spool.profile_id WHERE spool.used <> spool.weight AND spool.name ILIKE '%" +
                txn.esc(keyword) + "%' ORDER BY profile.vendor, profile.material, spool.name";

        pqxx::result resultSet{txn.exec(query)};

        auto items = std::vector<FilamentSpool>();

        for (auto row: resultSet) {
            items.emplace_back(FilamentSpool(
                    wxString::FromUTF8(row["name"].c_str()), wxString::FromUTF8(row["vendor"].c_str()),
                    wxString::FromUTF8(row["material"].c_str()), row["used"].as<double>(), row["weight"].as<double>(),
                    row["diameter"].as<float>(), row["cost"].as<float>()));
        }

        txn.commit();

        filamentSpoolDataViewListModel->Fill(items);
        dvlFilamentSpools->AssociateModel(filamentSpoolDataViewListModel);
    } catch (std::exception &exception) {
        wxLogStatus(exception.what());
    }
}
