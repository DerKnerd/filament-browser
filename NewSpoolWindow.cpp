//
// Created by imanuel on 8/14/22.
//

#include "NewSpoolWindow.h"
#include "ProfileListWindow.h"
#include "configuration.h"

NewSpoolWindow::NewSpoolWindow(wxWindow *parent) : wxDialog(parent, wxID_ANY, "Neue Filamentrolle", wxDefaultPosition,
                                                            wxSize(480, -1), wxDEFAULT_DIALOG_STYLE,
                                                            "NewSpoolWindow") {
    auto panel = new wxPanel(this);
    auto contentSizer = new wxBoxSizer(wxVERTICAL);
    panel->SetSizer(contentSizer);

    auto buttonSizer = new wxStdDialogButtonSizer();
    buttonSizer->AddButton(new wxButton(panel, wxID_OK, _("Filamentrolle speichern")));
    buttonSizer->AddButton(new wxButton(panel, wxID_CLOSE, _("Abbrechen")));
    SetAffirmativeId(wxID_OK);
    SetEscapeId(wxID_CLOSE);
    buttonSizer->Realize();

    auto itemsSizer = new wxFlexGridSizer(2);
    itemsSizer->SetFlexibleDirection(wxHORIZONTAL);
    itemsSizer->AddGrowableCol(1, 1);
    itemsSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

    auto nameLabel = new wxStaticText(panel, wxID_ANY, "Name");
    name = new wxTextCtrl(panel, wxID_ANY);
    name->SetValidator(wxTextValidator(wxFILTER_EMPTY));

    auto costLabel = new wxStaticText(panel, wxID_ANY, "Preis");
    cost = new wxSpinCtrlDouble(panel, wxID_ANY);
    cost->SetRange(0, 100000);

    auto weightLabel = new wxStaticText(panel, wxID_ANY, "Gewicht");
    weight = new wxSpinCtrlDouble(panel, wxID_ANY);
    weight->SetRange(0, 100000);
    weight->SetValue(1000);

    auto profiles = std::vector<FilamentProfile>();
    pqxx::connection connection{CONNECTION_STRING};
    pqxx::work txn{connection};
    auto query =
            "SELECT profile.id, profile.vendor, profile.material, profile.diameter, profile.density FROM profiles profile ORDER BY profile.vendor, profile.material";

    pqxx::result resultSet{txn.exec(query)};

    for (auto row: resultSet) {
        profiles.emplace_back(FilamentProfile(row["id"].as<int>(), wxString::FromUTF8(row["vendor"].c_str()),
                                              wxString::FromUTF8(row["material"].c_str()), row["density"].as<double>(),
                                              row["diameter"].as<double>()));
    }

    txn.commit();

    auto profileLabel = new wxStaticText(panel, wxID_ANY, "Profil");
    profile = new wxComboBox(panel, wxID_ANY);

    for (auto item: profiles) {
        profile->Append(wxString(item.vendor + " (" + item.material + ")"), &item);
    }

    itemsSizer->Add(nameLabel, 0, wxALL, WXC_FROM_DIP(5));
    itemsSizer->Add(name, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    itemsSizer->Add(costLabel, 0, wxALL, WXC_FROM_DIP(5));
    itemsSizer->Add(cost, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    itemsSizer->Add(weightLabel, 0, wxALL, WXC_FROM_DIP(5));
    itemsSizer->Add(weight, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));
    itemsSizer->Add(profileLabel, 0, wxALL, WXC_FROM_DIP(5));
    itemsSizer->Add(profile, 0, wxALL | wxEXPAND, WXC_FROM_DIP(5));

    contentSizer->Add(itemsSizer, 0, wxGROW | wxALL, 5);
    contentSizer->Add(buttonSizer, 0, wxGROW | wxALL);

    Layout();
    SetName("NewSpoolWindow");
    SetSize(wxDLG_UNIT(this, wxSize(-1, -1)));
    if (GetSizer()) {
        GetSizer()->Fit(panel);
    }
    if (GetParent()) {
        CentreOnParent(wxBOTH);
    } else {
        CentreOnScreen(wxBOTH);
    }

    Bind(wxEVT_BUTTON, [this, profiles](wxCommandEvent &) {
        if (this->Validate()) {
            pqxx::connection connection{CONNECTION_STRING};
            pqxx::work txn{connection};
            try {
                auto selection = profile->GetCurrentSelection();
                auto data = profiles[selection];
                auto query =
                        "INSERT INTO spools (profile_id, name, cost, weight) VALUES (" +
                        std::to_string(data.id) + ", '" +
                        txn.esc(name->GetValue().ToStdString()) + "', " +
                        std::to_string(cost->GetValue()) + ", " +
                        std::to_string(weight->GetValue()) + ")";
                txn.exec(query);
                txn.commit();
                AcceptAndClose();
            } catch (std::exception &exception) {
                wxMessageBox(_("Beim Speichern der Filamentrolle ist ein Fehler aufgetreten. " +
                               std::string(exception.what())), _("Speichern fehlgeschlagen"));
                txn.abort();
            }
        }
    }, wxID_OK);
}
