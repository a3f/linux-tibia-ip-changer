#include "gui.h"
#include "change.h"
#include "tibiapid.h"
#include "clientsversions.h"
#include "ico.xpm"

BEGIN_EVENT_TABLE(Panel,wxFrame)
	EVT_CLOSE(Panel::OnClose)
	EVT_MENU(wxID_EXIT, Panel::Quit)
	EVT_MENU(MENU_INFO, Panel::Info)
	EVT_BUTTON(wxID_EXIT, Panel::Quit)
	EVT_BUTTON(BUTTON_CHANGE_IP, Panel::guiChangeIP)
	EVT_BUTTON(BUTTON_TO_TRAY, Panel::toTray)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_TASKBAR_LEFT_DCLICK(MyTaskBarIcon::trayLeftClick)
    EVT_MENU(wxID_EXIT, MyTaskBarIcon::Quit)
    EVT_MENU(MENU_O_M, MyTaskBarIcon::miniOrMax)
END_EVENT_TABLE()

MyTaskBarIcon::MyTaskBarIcon(Panel* frame) 
: wxTaskBarIcon() 
, frame(frame) 
{ 
} 

Panel::Panel(const wxString& title)
       : wxFrame(NULL, wxID_ANY, title, wxPoint(-1, -1), wxSize(350, 130))
{
	panel = new wxPanel(this, wxID_ANY, wxPoint(-1, -1), wxSize(350, 130));
	SetMinSize(wxSize(350, 130));
	SetMaxSize(wxSize(350, 130));
	int ile = int(sizeof(clients)/sizeof(clients[1]));
	wxString tab[ile];
	for(int i = 0; i < ile; i++){
		wxString wxs(clients[i][0].insert(1, ".").c_str(), wxConvUTF8);
		tab[i] = wxs;
	}
	tibiaversion = new wxComboBox(panel, -1, tab[0], wxPoint(274, 20), wxSize(65, 25), WXSIZEOF(tab), tab, wxCB_READONLY);
	statusbar = new wxStatusBar(this, wxID_ANY);
	statusbar->SetFieldsCount(1);
	int stat[1];
	stat[0] = -1;
	statusbar->SetStatusWidths(1,stat);
	SetStatusBar(statusbar);
	statictext = new wxStaticText(panel, wxID_ANY, wxT("IP adress:                                         Port:    Tibia version:"), wxPoint(5, 5), wxSize(350, 25));
	ip = new wxTextCtrl(panel, wxID_ANY, wxT(""), wxPoint(5, 20), wxSize(214, 25));
	port = new wxTextCtrl(panel, wxID_ANY, wxT("7171"), wxPoint(227, 20), wxSize(40, 25));
	port->SetMaxLength(4);
	menubar = new wxMenuBar;
	file = new wxMenu;
	file->Append(wxID_EXIT, wxT("&Quit"));
	menubar->Append(file, wxT("&File"));
	help = new wxMenu;
	help->Append(MENU_INFO, wxT("&Info"));
	menubar->Append(help, wxT("&Help"));
	SetMenuBar(menubar);
	trayIcon = new MyTaskBarIcon(this);
	wxIcon icon(ico_xpm);
	if(!trayIcon->SetIcon(icon, wxT("IP Chabger")))
        wxMessageBox(wxT("Could not set icon."));
    wxButton *cIp = new wxButton(panel, BUTTON_CHANGE_IP, wxT("Change IP"), wxPoint(80, 50), wxSize(80, 33));
    wxButton *bTray = new wxButton(panel, BUTTON_TO_TRAY, wxT("Add to Tray"), wxPoint(170, 50), wxSize(80, 33));
    wxButton *bQuit = new wxButton(panel, wxID_EXIT, wxT("Quit"), wxPoint(260, 50), wxSize(80, 33));
	
	this->SetIcon(icon);
	Centre();
}

void Panel::OnClose(wxCloseEvent& event){
	Iconize(true);
	Show(false);
}

void Panel::Quit(wxCommandEvent& event){ 
	trayIcon->RemoveIcon();
	Destroy();
}

void MyTaskBarIcon::Quit(wxCommandEvent& event){ 
	RemoveIcon();
	frame->Destroy();
}

void MyTaskBarIcon::trayLeftClick(wxTaskBarIconEvent& event){
	if(frame->IsIconized()){
		frame->Iconize(false);
		frame->Show(true);
	}else{
		frame->Iconize(true);
		frame->Show(false);	
	}
}

void MyTaskBarIcon::miniOrMax(wxCommandEvent& event){
	if(frame->IsIconized()){
		frame->Iconize(false);
		frame->Show(true);
	}else{
		frame->Iconize(true);
		frame->Show(false);	
	}
}

void Panel::toTray(wxCommandEvent& event){
	Iconize(true);
	Show(false);	
}

wxMenu* MyTaskBarIcon::CreatePopupMenu() 
{ 
    menuTray = new wxMenu(); 

	if(frame->IsIconized())
		menuTray->Append(MENU_O_M, wxT("Open"));
	else
		menuTray->Append(MENU_O_M, wxT("Minimize"));
    menuTray->AppendSeparator(); 
    menuTray->Append(wxID_EXIT, wxT("&Quit")); 

    return menuTray; 
}

void Panel::guiChangeIP(wxCommandEvent& event){
	TibiaPid pid;
	int tpid = pid.getTibiaPid();
	if(tpid == 0){
		statusbar->SetStatusText(wxT("Tibia process not found!"),0);
		return;
	}

    Change c(tpid);
    std::string sip = std::string(ip->GetValue().mb_str());
    wxString wxIp(sip.c_str(), wxConvUTF8);
    std::string sport = std::string(port->GetValue().mb_str());
    std::string client = std::string(tibiaversion->GetValue().mb_str());
	std::string s ="";
	s += client.at(1);
	if(s == ".")
		client = client.erase(1,1);

    bool ret = c.changeIP(sip, atoi(sport.c_str()), client);
    if(ret)
		statusbar->SetStatusText(wxT("IP changed!"),0);
	else
		statusbar->SetStatusText(wxT("IP not changed!"),0);
}

void Panel::Info(wxCommandEvent& event){
	wxMessageDialog *dial = new wxMessageDialog(NULL, wxT("This program change IP tibia client in order to connect to Open Tibia Server.\nContinue Moraxus project.\nLicense: GNU GPL\nProgrammer: Miziak.\nAdress Finder: Virtelio.\nWrote in wxWidgets library."), wxT("Error"), wxOK);
	dial->ShowModal();
}
