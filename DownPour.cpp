#include "SDL.h"
#include "GameData.h"
#include "System.h"
#include "ImageManip.h"
#include "GUILib.h"

SDL_Surface *screen=NULL;
GameData *Gamedata=NULL;

void InitVideo();
void Menus();

int TitleMenu();
int FieldsMenu();
int ChatRmMenu();
int HiScoresMenu();
int SettingsMenu();
int ServerMenu();
SDL_Surface* ResizeImage(char* filename);

void AdvanceFrame();
void TimedEvents();
void InitBoardImages(GUI* gui);

void RenderBoards(GUI* gui);
void RenderNicks(GUI* gui);
void RenderIncoming(GUI* gui);
void RenderPiece(Piece* piece);
void RenderSPiece(Piece* piece);
void RenderLClear(int frame);
void RenderSClear(int frame);
void RenderHighScores(GUI* gui);

int main (int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        Error(SDL_GetError());
        exit(0);       
    }
	SDL_EnableKeyRepeat(100,100);
	SDL_EnableUNICODE(1);

    Gamedata=new GameData();
    
    InitVideo();
    Menus();
    
    if (Gamedata->network->Client) {
        Gamedata->network->SendStatus(PLEAVE,Gamedata->Pnum);
        Gamedata->network->ClientDiscon();
    }
        
    if (Gamedata->network->Server) {
        Gamedata->scores->SaveScores();
        Gamedata->network->ServerDiscon();
    }
        
    delete Gamedata;
    SDL_Quit();
    return 0;
}

void InitVideo()
{
    // Check for fullscreen then check if the machine can handel the specified video mode
    if (Gamedata->settings->FULLSCREEN == 0) {
        if (SDL_VideoModeOK(Gamedata->settings->X,Gamedata->settings->Y, Gamedata->settings->BBP, 
            SDL_SWSURFACE) == 0) {
            Error(SDL_GetError());
            exit(0);
        }
        screen=SDL_SetVideoMode(Gamedata->settings->X,Gamedata->settings->Y, Gamedata->settings->BBP,SDL_SWSURFACE);
    }
    else {
        if (SDL_VideoModeOK(Gamedata->settings->X,Gamedata->settings->Y, Gamedata->settings->BBP, 
            SDL_SWSURFACE|SDL_FULLSCREEN) == 0) {
            Error(SDL_GetError());
            exit(0);
        }
        screen=SDL_SetVideoMode(Gamedata->settings->X,Gamedata->settings->Y, Gamedata->settings->BBP,SDL_SWSURFACE|SDL_FULLSCREEN);
    }
    return;
}

void Menus()
{
    int menu=0;
    while (menu != 999) {
        switch (menu) {
            case 0:
                menu=TitleMenu();
                break;
            case 1:
                menu=FieldsMenu();
                break;
            case 2:
                menu=ChatRmMenu();
                break;
            case 3:
                menu=HiScoresMenu();
                break;
            case 4:
                menu=SettingsMenu();
                break;
            case 5:
                menu=ServerMenu();
                break;
            case 6:
                menu=999;
                break;
            default:
                menu=TitleMenu();
                break;
        }
    }
    return;
}

int TitleMenu()
{
    int ClickedOn=0;
    double xoff=double(Gamedata->settings->X)/640;
    double yoff=double(Gamedata->settings->Y)/480;
    
    GUI Title(screen,ResizeImage("Title.bmp"));
	Title.AddButton(Button::pushbutton,1,int(10*xoff),int(5*yoff),ResizeImage("FieldsB.bmp"));
	Title.AddButton(Button::pushbutton,2,int(114*xoff),int(5*yoff),ResizeImage("ChatRMB.bmp"));
	Title.AddButton(Button::pushbutton,3,int(218*xoff),int(5*yoff),ResizeImage("HighScoresB.bmp"));
	Title.AddButton(Button::pushbutton,4,int(322*xoff),int(5*yoff),ResizeImage("SettingsB.bmp"));
	Title.AddButton(Button::pushbutton,5,int(426*xoff),int(5*yoff),ResizeImage("ConnectB.bmp"));
	Title.AddButton(Button::pushbutton,6,int(530*xoff),int(5*yoff),ResizeImage("ServerB.bmp"));

    bool fin=false;
    SDL_Event event;

	while(!fin){
        SDL_Delay(10);
        while (SDL_PollEvent(&event)) { 
            ClickedOn=Title.ProcessEvent(&event);

            if (event.type == SDL_QUIT || ClickedOn == 6) {
                fin=true;
                ClickedOn=999;
            }
            if (ClickedOn >= 1 && ClickedOn <= 5)
                fin=true;
        }
        TimedEvents();
        Title.Display();
	}
    return ClickedOn; 
}

int FieldsMenu()
{
    double xoff=double(Gamedata->settings->X)/640;
    double yoff=double(Gamedata->settings->Y)/480;
    Gamedata->settings->FrameRate.Activate();
    int ClickedOn=0;

    GUI Fields(screen,ResizeImage("Fields.bmp"));
    Fields.AddFont("times.ttf",1,int(18*xoff));
    Fields.AddFont("times.ttf",2,int(12*xoff));
	Fields.AddButton(Button::pushbutton,1,int(10*xoff),int(420*yoff),ResizeImage("FieldsB.bmp"));
	Fields.AddButton(Button::pushbutton,2,int(114*xoff),int(420*yoff),ResizeImage("ChatRMB.bmp"));
	Fields.AddButton(Button::pushbutton,3,int(218*xoff),int(420*yoff),ResizeImage("HighScoresB.bmp"));
	Fields.AddButton(Button::pushbutton,4,int(322*xoff),int(420*yoff),ResizeImage("SettingsB.bmp"));
	Fields.AddButton(Button::pushbutton,5,int(426*xoff),int(420*yoff),ResizeImage("ConnectB.bmp"));
	Fields.AddButton(Button::pushbutton,6,int(530*xoff),int(420*yoff),ResizeImage("ServerB.bmp"));

	InitBoardImages(&Fields);
    RenderNicks(&Fields);
        
    bool fin=false;
    SDL_Event event;

	while(!fin){
        SDL_Delay(10);
        while (SDL_PollEvent(&event)) { 
            ClickedOn=Fields.ProcessEvent(&event);

            if (event.type == SDL_QUIT || ClickedOn == 6) {
                fin=true;
                ClickedOn=999;
            }
            if (ClickedOn >= 2 && ClickedOn <= 5)
                fin=true;
                
			if (Gamedata->GameOn) {
				if (event.key.state == SDL_PRESSED) {
					switch (event.key.keysym.sym) {
						case SDLK_1: Gamedata->network->SendSpecial(0,Gamedata->Pnum); break;
						case SDLK_2: Gamedata->network->SendSpecial(1,Gamedata->Pnum); break;
						case SDLK_3: Gamedata->network->SendSpecial(2,Gamedata->Pnum); break;
						case SDLK_4: Gamedata->network->SendSpecial(3,Gamedata->Pnum); break;
						case SDLK_5: Gamedata->network->SendSpecial(4,Gamedata->Pnum); break;
						case SDLK_6: Gamedata->network->SendSpecial(5,Gamedata->Pnum); break;
						case SDLK_UP: Gamedata->boards[Gamedata->Pnum]->RotateRight(); break;
						case SDLK_DOWN: Gamedata->boards[Gamedata->Pnum]->RotateLeft(); break;
						case SDLK_RIGHT: Gamedata->boards[Gamedata->Pnum]->MoveRight(); break;
						case SDLK_LEFT: Gamedata->boards[Gamedata->Pnum]->MoveLeft(); break;
						case SDLK_SPACE: Gamedata->boards[Gamedata->Pnum]->MovePiece(); break;
					}
                }			
			}
        }
        TimedEvents();
        if (Gamedata->settings->FrameRate.Ready()) {
		        AdvanceFrame();
		        RenderBoards(&Fields); 
        }
        if (Gamedata->UpdateNicks) {
            RenderNicks(&Fields);
            RenderIncoming(&Fields);
        }
        Fields.Display();
	}
	Gamedata->settings->FrameRate.Deactivate();
    return ClickedOn; 
}

int ChatRmMenu()
{
    double xoff=double(Gamedata->settings->X)/640;
    double yoff=double(Gamedata->settings->Y)/480;
    int ClickedOn=0;

    GUI ChatRM(screen,ResizeImage("ChatRM.bmp"));
    ChatRM.AddFont("times.ttf",1,int(18*xoff));
	ChatRM.AddButton(Button::pushbutton,1,int(10*xoff),int(5*yoff),ResizeImage("FieldsB.bmp"));
	ChatRM.AddButton(Button::pushbutton,2,int(114*xoff),int(5*yoff),ResizeImage("ChatRMB.bmp"));
	ChatRM.AddButton(Button::pushbutton,3,int(218*xoff),int(5*yoff),ResizeImage("HighScoresB.bmp"));
	ChatRM.AddButton(Button::pushbutton,4,int(322*xoff),int(5*yoff),ResizeImage("SettingsB.bmp"));
	ChatRM.AddButton(Button::pushbutton,5,int(426*xoff),int(5*yoff),ResizeImage("ConnectB.bmp"));
	ChatRM.AddButton(Button::pushbutton,6,int(530*xoff),int(5*yoff),ResizeImage("ServerB.bmp"));
	ChatRM.AddTxtWin(int(10*xoff),int(107*yoff),int(429*yoff),int(341*yoff),500,10,ChatRM.GetFontPtr(1),true);
	ChatRM.ChangeTxtWinInputCords(10,int(10*xoff),int(449*yoff),int(620*xoff));
    // Add the nicks  
    for (int i=0;i < 6;i++) {
        SDL_Surface* temp;
        int x=int(465*xoff);
        int y=int((107*yoff)+(i*18*xoff));
        temp=SDL_CreateRGBSurface(SDL_SWSURFACE, int(Gamedata->images->SingleP->w*6*xoff),int(19*yoff),screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
        ChatRM.AddButton(Button::pushbutton,i + 20,x,y,temp);
    }    
    RenderNicks(&ChatRM);
         
	if (!Gamedata->GameOn) 
	   ChatRM.AddButton(Button::pushbutton,7,int(10*xoff),int(55*yoff),ResizeImage("StartGB.bmp"));
	else
	   ChatRM.AddButton(Button::pushbutton,7,int(10*xoff),int(55*yoff),ResizeImage("StopGB.bmp"));
  
    bool fin=false;
    SDL_Event event;

	while(!fin){
        SDL_Delay(10);
        // If ChatMSGS.size() > 0 then we have incoming text messages to be displayed
        if (Gamedata->ChatMSGS.size() > 0) {
            for (int i=0; i < Gamedata->ChatMSGS.size();i++) {
                Text* temp=new Text(Gamedata->ChatMSGS[i],0,0,0);
                if (Gamedata->ChatMSGS[i][0]=='*')
                    temp->SetColor(0,0,255);
                ChatRM.AddTxtToWin(temp,10);
                delete [] Gamedata->ChatMSGS[i];  
            }
            Gamedata->ChatMSGS.clear();
        }
        
        while (SDL_PollEvent(&event)) {
            // If enter is pressed and the txt window is active then we store/send the text
            if (event.key.state == SDL_PRESSED && event.key.keysym.sym == SDLK_RETURN)
                if (ChatRM.CheckActiveTxtWin(10) == true) {
                    Gamedata->network->SendText(ChatRM.GetTxtWinText(10),Network_H::MSGINCHATRM,Gamedata->Pnum);
                    if (Gamedata->network->Server) {
                        char *temp=new char[strlen(Gamedata->settings->Nicks[Gamedata->Pnum]) +
                            strlen(ChatRM.GetTxtWinText(10)) + 3];
                        sprintf(temp,"%s: %s%c",Gamedata->settings->Nicks[Gamedata->Pnum],ChatRM.GetTxtWinText(10),'\0');
                        Gamedata->ChatMSGS.insert(Gamedata->ChatMSGS.end(),temp); 
                    }
                    ChatRM.ChangeTextWinBText(10,NULL);
                }
            ClickedOn=ChatRM.ProcessEvent(&event);

            if (event.type == SDL_QUIT || ClickedOn == 6) {
                fin=true;
                ClickedOn=999;
            }
            if (ClickedOn >= 1 && ClickedOn <= 5 && ClickedOn != 2)
                fin=true;
            
            if (ClickedOn == 7)
                    if ((Gamedata->network->Server || Gamedata->network->Client) && !Gamedata->GameOn) {
                        Gamedata->network->SendStatus(STARTGAME,0);
                        ChatRM.ChangeButtonSurface(7,ResizeImage("StopGB.bmp"));
                    }
                else 
                    if (Gamedata->network->Server || Gamedata->network->Client) {
                        Gamedata->network->SendStatus(STOPGAME,0);
                        ChatRM.ChangeButtonSurface(7,ResizeImage("StartGB.bmp"));
                    }
        }
        TimedEvents();
        
        if (Gamedata->UpdateNicks)
            RenderNicks(&ChatRM);
        ChatRM.Display();
	}
	// We're leaving the chatroom so store all text from the window in the chatroom buffer
    int size=ChatRM.GetTxtWinSize(10);
    for (int i=0; i < size;++i) {
        char *temp=new char[strlen(ChatRM.GetTxtWinText(10,i))+1];
        strcpy(temp,ChatRM.GetTxtWinText(10,i));
        Gamedata->ChatMSGS.insert(Gamedata->ChatMSGS.end(),temp);
    }
    return ClickedOn; 
}

int HiScoresMenu()
{
    double xoff=double(Gamedata->settings->X)/640;
    double yoff=double(Gamedata->settings->Y)/480;
    int ClickedOn=0;
    
    GUI Highs(screen,ResizeImage("HighScores.bmp"));
    Highs.AddFont("times.ttf",1,int(18*xoff));
	Highs.AddButton(Button::pushbutton,1,int(10*xoff),int(5*yoff),ResizeImage("FieldsB.bmp"));
	Highs.AddButton(Button::pushbutton,2,int(114*xoff),int(5*yoff),ResizeImage("ChatRMB.bmp"));
	Highs.AddButton(Button::pushbutton,3,int(218*xoff),int(5*yoff),ResizeImage("HighScoresB.bmp"));
	Highs.AddButton(Button::pushbutton,4,int(322*xoff),int(5*yoff),ResizeImage("SettingsB.bmp"));
	Highs.AddButton(Button::pushbutton,5,int(426*xoff),int(5*yoff),ResizeImage("ConnectB.bmp"));
	Highs.AddButton(Button::pushbutton,6,int(530*xoff),int(5*yoff),ResizeImage("ServerB.bmp"));
	
	char *temp;
	for (int i=0; i < 10;++i) {
	   temp=new char[20];
	   sprintf(temp,"%d. Nobody - 0%c",i,'\0');
	   Highs.AddText(temp,i+10,int(10*xoff),int(100*yoff+(18*i)),1);
	   delete [] temp;
    }
    
    if (Gamedata->network->Client)
        Gamedata->network->RequestHighScores();
    if (Gamedata->network->Server)
        RenderHighScores(&Highs);
        
    bool fin=false;
    SDL_Event event;

	while(!fin){
        SDL_Delay(10);
        while (SDL_PollEvent(&event)) { 
            ClickedOn=Highs.ProcessEvent(&event);

            if (event.type == SDL_QUIT || ClickedOn == 6) {
                fin=true;
                ClickedOn=999;
            }
            if (ClickedOn >= 1 && ClickedOn <= 5 && ClickedOn != 3)
                fin=true;
        }
        if (Gamedata->UpdateScores)
            RenderHighScores(&Highs);
            
        TimedEvents();
        Highs.Display();
	}
    return ClickedOn; 
}

int SettingsMenu()
{
    double xoff=double(Gamedata->settings->X)/640;
    double yoff=double(Gamedata->settings->Y)/480;
    int ClickedOn=0;
        SDL_Surface *bg=SDL_LoadBMP("Settings.bmp");
    if (bg->w != Gamedata->settings->X || bg->h != Gamedata->settings->Y) {
        SDL_Surface* temp=ResizeImageToFit(bg,Gamedata->settings->X,Gamedata->settings->Y);
        SDL_FreeSurface(bg);
        bg=temp;
    } 
    GUI Settings(screen,ResizeImage("Settings.bmp"));
	Settings.AddButton(Button::pushbutton,1,int(10*xoff),int(420*yoff),ResizeImage("FieldsB.bmp"));
	Settings.AddButton(Button::pushbutton,2,int(114*xoff),int(420*yoff),ResizeImage("ChatRMB.bmp"));
	Settings.AddButton(Button::pushbutton,3,int(218*xoff),int(420*yoff),ResizeImage("HighScoresB.bmp"));
	Settings.AddButton(Button::pushbutton,4,int(322*xoff),int(420*yoff),ResizeImage("SettingsB.bmp"));
	Settings.AddButton(Button::pushbutton,5,int(426*xoff),int(420*yoff),ResizeImage("ConnectB.bmp"));
	Settings.AddButton(Button::pushbutton,6,int(530*xoff),int(420*yoff),ResizeImage("ServerB.bmp"));
	
    bool fin=false;
    SDL_Event event;

	while(!fin){
        SDL_Delay(10);
        while (SDL_PollEvent(&event)) { 
            ClickedOn=Settings.ProcessEvent(&event);

            if (event.type == SDL_QUIT || ClickedOn == 6) {
                fin=true;
                ClickedOn=999;
            }
            if (ClickedOn >= 1 && ClickedOn <= 5 && ClickedOn != 4)
                fin=true;
        }
        TimedEvents();
        Settings.Display();
	}
    return ClickedOn; 
}

int ServerMenu()
{
    double xoff=double(Gamedata->settings->X)/640;
    double yoff=double(Gamedata->settings->Y)/480;
    int ClickedOn=0;

    GUI Connect(screen,ResizeImage("Server.bmp"));
    Connect.AddFont("times.ttf",1,int(18*xoff));
	Connect.AddButton(Button::pushbutton,1,int(10*xoff),int(420*yoff),ResizeImage("FieldsB.bmp"));
	Connect.AddButton(Button::pushbutton,2,int(114*xoff),int(420*yoff),ResizeImage("ChatRMB.bmp"));
	Connect.AddButton(Button::pushbutton,3,int(218*xoff),int(420*yoff),ResizeImage("HighScoresB.bmp"));
	Connect.AddButton(Button::pushbutton,4,int(322*xoff),int(420*yoff),ResizeImage("SettingsB.bmp"));
	Connect.AddButton(Button::pushbutton,5,int(426*xoff),int(420*yoff),ResizeImage("ConnectB.bmp"));
	Connect.AddButton(Button::pushbutton,6,int(530*xoff),int(420*yoff),ResizeImage("ServerB.bmp"));
	Connect.AddButton(Button::pushbutton,7,int(100*xoff),int(100*yoff),ResizeImage("ServerB.bmp"));
	Connect.AddButton(Button::pushbutton,8,int(400*xoff),int(100*yoff),ResizeImage("ConnectB.bmp"));
	
	Connect.AddTextInput(10, int(58*xoff), int(235*yoff), int(140*xoff), 1,Gamedata->settings->MyNick);
	Connect.AddTextInput(11, int(121*xoff), int(290*yoff), int(140*xoff), 1,"localhost");
	
    bool fin=false;
    SDL_Event event;

	while(!fin){
        SDL_Delay(10);
        while (SDL_PollEvent(&event)) {
            if (event.key.state == SDL_PRESSED && event.key.keysym.sym == SDLK_RETURN) {
                if (Connect.CheckActiveButton(10)) {
                    Gamedata->network->SetNick(Connect.GetButtonText(10),Gamedata->Pnum);
                    if (Gamedata->settings->MyNick != NULL)
                        delete [] Gamedata->settings->MyNick;
                    Gamedata->settings->MyNick=new char[strlen(Connect.GetButtonText(10))+1];
                    strcpy(Gamedata->settings->MyNick,Connect.GetButtonText(10));
                    Gamedata->network->SendNickToAll(Gamedata->Pnum);
                    Connect.SetButtonActive(10,false);
                }
                if (Connect.CheckActiveButton(11))
                    Connect.SetButtonActive(11,false);
            }  
            ClickedOn=Connect.ProcessEvent(&event);
            if (event.type == SDL_QUIT || ClickedOn == 6) {
                fin=true;
                ClickedOn=999;
            }
            if (ClickedOn >= 1 && ClickedOn <= 4)
                fin=true;
                
            if (ClickedOn == 7) 
                Gamedata->network->HostGame();
                           
            if (ClickedOn == 8) 
                Gamedata->network->ClientConnect(Connect.GetButtonText(11));
            
        }
        TimedEvents();
        Connect.Display();
	}
    return ClickedOn; 
}

SDL_Surface* ResizeImage(char* filename)
{
    SDL_Surface *image=SDL_LoadBMP(filename);
    double xoff=double(Gamedata->settings->X)/640;
    double yoff=double(Gamedata->settings->Y)/480;      
    SDL_Surface* temp=ResizeImageToFit(image,int(image->w*xoff),int(image->h*yoff));
        if (image != NULL)
            SDL_FreeSurface(image);
        image=temp;
    return image;	
}

void AdvanceFrame()
{
    for (int i=0; i < 6; ++i)
        for (int y=0; y < 12;++y)
            for (int x=0; x < 6;++x) 
                if (Gamedata->Pnum == i) {
                    if (Gamedata->boards[i]->board[x][y] != NULL) {
                        Gamedata->boards[i]->board[x][y]->frame++;
                        if (Gamedata->boards[i]->board[x][y]->frame > Gamedata->images->Frames-1)
                            Gamedata->boards[i]->board[x][y]->frame = 0;
                    }
                }
                else {
                    if (Gamedata->boards[i]->board[x][y] != NULL) {
                        Gamedata->boards[i]->board[x][y]->frame++;
                        if (Gamedata->boards[i]->board[x][y]->frame > Gamedata->images->SFrames-1)
                            Gamedata->boards[i]->board[x][y]->frame = 0;
                    }
                }
                
    if (Gamedata->GameOn) {            
        for (int i=0; i < 2;++i) {
            Gamedata->boards[Gamedata->Pnum]->CurPiece[i].frame++;
            if (Gamedata->boards[Gamedata->Pnum]->CurPiece[i].frame > Gamedata->images->Frames-1)
                Gamedata->boards[Gamedata->Pnum]->CurPiece[i].frame=0;
        }
        for (int i=0; i < 2;++i) {
            Gamedata->boards[Gamedata->Pnum]->NxtPiece[i].frame++;
            if (Gamedata->boards[Gamedata->Pnum]->NxtPiece[i].frame > Gamedata->images->Frames-1)
                Gamedata->boards[Gamedata->Pnum]->NxtPiece[i].frame=0;
        }
    }  
    return;   
}

void TimedEvents()
{
    if (Gamedata->GameOn)
        if (Gamedata->boards[Gamedata->Pnum]->board[3][0] != NULL) {
            Gamedata->network->SendStatus(OUTOFGAME,Gamedata->Pnum);
            Gamedata->GameOn=false;		
            Gamedata->settings->DropRate.Deactivate();
            Gamedata->settings->IncreaseDropRate.Deactivate();
            Gamedata->boards[Gamedata->Pnum]->FillBoardWClears();
    }
            
	if (Gamedata->settings->DropRate.Ready()) 
			Gamedata->boards[Gamedata->Pnum]->MovePiece(); 	
		
	if (Gamedata->network->PacketRate.Ready()) {
		if (Gamedata->network->Server)
			Gamedata->network->PackServ();
		if (Gamedata->network->Client)
			Gamedata->network->PackClient();
	}
	
    // Used to increase the speed in which a piece drops		
	if (Gamedata->settings->IncreaseDropRate.Ready()) 
		Gamedata->settings->DropRate.ChangeDelay(Gamedata->settings->DropRate.GetDelay() /2);
	
	for (int i=0; i < 6;++i) {
	   if (Gamedata->boards[i]->FloatTime.Ready()) {
            Gamedata->boards[i]->FloatTime.Deactivate();
            Gamedata->boards[i]->FloatTime.ChangeDelay(Gamedata->settings->floattime);
            Gamedata->boards[i]->DropPieces();
        }
        if (Gamedata->boards[i]->ClearDropRate.Active() && Gamedata->boards[i]->ClearDropRate.TimeLeft() == 0) 
            Gamedata->boards[i]->DropClears(0,0);       
    }     
    return;
}

void InitBoardImages(GUI* gui)
{
    // Calc the size of each board and positions
    double xoffset=double(Gamedata->settings->X)/640;
    double yoffset=double(Gamedata->settings->Y)/480;
    int psize=Gamedata->images->SingleP->w;
    SDL_Surface* Large, *Small[5];
    Large=SDL_CreateRGBSurface(SDL_SWSURFACE, psize*6, psize*12, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
    for (int i=0;i < 5;++i)
        Small[i]=SDL_CreateRGBSurface(SDL_SWSURFACE, psize*3, psize*6, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
    
    gui->AddButton(Button::pushbutton, Gamedata->Pnum * 100 + 100, int(5*xoffset),int(26*yoffset),Large);
    
    // Using pos so we can keep the player number related to the board's button id number
    // player number = (button board id / 100) - 1
    int pos=0;
    for (int i=0;i < 5;++i) {
        if (Gamedata->Pnum == i)
            pos++;
            int x=215+ ((i % 3) * 150) + ((i/3) * 150);
            int y=26 + ((i/3)*202);
            gui->AddButton(Button::pushbutton,100 * pos + 100,int(x*xoffset),int(y*yoffset),Small[i]);
        pos++;       
    }
    // Add the Next Piece 
    SDL_Surface *temp=NULL;
    for (int i=0; i < 2;++i) {
        int x=185; int y=5 + (i * Gamedata->images->SingleP->h);
        gui->AddButton(Button::pushbutton,i+10,int(x*xoffset),int(y*yoffset),temp);
    }
    
    // Add the Specials button
    temp=SDL_CreateRGBSurface(SDL_SWSURFACE, Gamedata->images->Specials->w, Gamedata->images->Specials->h, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
        gui->AddButton(Button::pushbutton,15,int(5*xoffset),int(387*yoffset),temp);
        
    // Add the nicks
    for (int i=0;i < 6;i++) {
        int x=gui->GetBXcord(100*i+100);
        int y=gui->GetBYcord(100*i+100)-20;
        if (Gamedata->Pnum == i) {
            temp=SDL_CreateRGBSurface(SDL_SWSURFACE, int(Gamedata->images->SingleP->w*6*xoffset),int(19*yoffset),screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
            gui->AddButton(Button::pushbutton,i + 20,x,y,temp);
        }
        else {
            temp=SDL_CreateRGBSurface(SDL_SWSURFACE, int(Gamedata->images->SSingleP->w*6*xoffset),int(19*yoffset),screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
            gui->AddButton(Button::pushbutton,i + 20, x,y,temp);
        }
    }
    return;
}

void RenderNicks(GUI* gui)
{
    SDL_Rect src;
    char temptxt[512];
    Text temp(NULL,1,0,0);
    for (int i=0; i < 6;++i) {
        if (Gamedata->settings->Nicks[i] == NULL)
            sprintf(temptxt,"%d.%c",i+1,'\0'); 
        else
            sprintf(temptxt,"%d. %s%c",i+1, Gamedata->settings->Nicks[i],'\0');     
            
        temp.ChangeText(temptxt);
        src.x=gui->GetBXcord(20+i); src.y=gui->GetBYcord(20+i);
        src.w=gui->GetButtonSurface(20+i)->w; src.h=gui->GetButtonSurface(20+i)->h;
        SDL_BlitSurface(gui->GetBG(),&src,gui->GetButtonSurface(20+i),NULL);
        temp.RenderText(gui->GetFont(1));
        SDL_BlitSurface(temp.RTEXT,NULL,gui->GetButtonSurface(20+i),NULL);
        gui->ReDrawButton(20+i);
    }
    Gamedata->UpdateNicks=false;
    return;
}

void RenderIncoming(GUI* gui)
{
    int C2Drop;
    int tlines=0; int slines=0; int singles=0;
    SDL_Rect dest;
    int w=gui->GetButtonSurface(20+Gamedata->Pnum)->w;
    int h=gui->GetButtonSurface(20+Gamedata->Pnum)->h;
    SDL_Surface* tsurf=SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, screen->format->BitsPerPixel, screen->format->Rmask, screen->format->Gmask,screen->format->Bmask,screen->format->Amask);
    
    for (int j=0; j <6;++j) { 
        SDL_FillRect(tsurf,NULL,SDL_MapRGB(tsurf->format,255,0,255));
        SDL_SetColorKey(tsurf, SDL_SRCCOLORKEY, SDL_MapRGB(tsurf->format,255,0,255));
        C2Drop=Gamedata->boards[j]->Num2Drop;
        dest.x=0; dest.y=0;
        tlines=C2Drop / 18; C2Drop=C2Drop % 18;
        slines=C2Drop / 6; C2Drop=C2Drop % 6;
        singles=C2Drop;
        Piece temp(0,0,0);
        for (int i=0; i < tlines;++i) {
            temp.color=1;
            RenderPiece(&temp);
            dest.w=Gamedata->images->SingleP->w; dest.h=Gamedata->images->SingleP->h;
            SDL_BlitSurface(Gamedata->images->SingleP,NULL,tsurf,&dest);
            dest.x=dest.x+Gamedata->images->SingleP->w;
        }
            for (int i=0; i < slines;++i) {
            temp.color=0;
            RenderPiece(&temp);
            dest.w=Gamedata->images->SingleP->w; dest.h=Gamedata->images->SingleP->h;
            SDL_BlitSurface(Gamedata->images->SingleP,NULL,tsurf,&dest);
            dest.x=dest.x+Gamedata->images->SingleP->w;
        }
            for (int i=0; i < singles;++i) {
            temp.color=0;
            RenderSPiece(&temp);
            dest.w=Gamedata->images->SSingleP->w; dest.h=Gamedata->images->SSingleP->h;
            SDL_BlitSurface(Gamedata->images->SSingleP,NULL,tsurf,&dest);
            dest.x=dest.x+Gamedata->images->SSingleP->w;
        }
        SDL_BlitSurface(tsurf,NULL,gui->GetButtonSurface(20+j),NULL);
    }
    if (tsurf != NULL)
        SDL_FreeSurface(tsurf);
    Gamedata->UpdateNicks=false;
    return;
}

void RenderBoards(GUI* gui)
{
	SDL_Rect src,dest;
	int x,y;
	SDL_Surface* bg=gui->GetBG();
	
    // Redraw the background for each board
	src.w=Gamedata->images->SSingleP->w*6; src.h=Gamedata->images->SSingleP->w*12;
    for (int i=0;i < 6;++i) {
        src.x=gui->GetBXcord(i * 100 + 100);
        src.y=gui->GetBYcord(i * 100 + 100);
        if (Gamedata->Pnum == i) { 	// Reblit the background for the large board
            SDL_Rect src2; src2.x=src.x; src2.y=src.y;
            src2.w=Gamedata->images->SingleP->w*6; src2.h=Gamedata->images->SingleP->w*12;
            SDL_BlitSurface(bg,&src2,gui->GetButtonSurface(i * 100 + 100),NULL);
        }
        else    // Blit the background for the small boards
            SDL_BlitSurface(bg,&src,gui->GetButtonSurface(i * 100 + 100),NULL);
        gui->ReDrawButton(i*100+100); 
    }
    
    // Now draw the pieces
    for (int i=0;i < 6;++i) 
        for (int y=0;y < 12;++y) 
            for (int x=0; x < 6;++x) 
                if (Gamedata->boards[i]->board[x][y] != NULL)
                    if (Gamedata->Pnum == i) {  // Drawing Large Pieces
                        dest.w=Gamedata->images->SingleP->w;
                        dest.h=Gamedata->images->SingleP->h;
                        RenderPiece(Gamedata->boards[Gamedata->Pnum]->board[x][y]);
                        dest.x=Gamedata->boards[Gamedata->Pnum]->board[x][y]->x * Gamedata->images->SingleP->w;
                        dest.y=Gamedata->boards[Gamedata->Pnum]->board[x][y]->y * Gamedata->images->SingleP->h;
                        SDL_BlitSurface(Gamedata->images->SingleP,NULL,gui->GetButtonSurface(Gamedata->Pnum * 100 + 100),&dest);
                    }
                    else {  // Drawing small pieces
                    	dest.w=Gamedata->images->SSingleP->w;
                        dest.h=Gamedata->images->SSingleP->h;
                        RenderSPiece(Gamedata->boards[i]->board[x][y]);
                        dest.x=Gamedata->boards[i]->board[x][y]->x * Gamedata->images->SSingleP->w;
                        dest.y=Gamedata->boards[i]->board[x][y]->y * Gamedata->images->SSingleP->h;
                        SDL_BlitSurface(Gamedata->images->SSingleP,NULL,gui->GetButtonSurface(i * 100 + 100),&dest);
                    }
            
    if (Gamedata->GameOn) {
        // Render the current piece
        for (int i=0; i < 2;++i) {
            RenderPiece(&Gamedata->boards[Gamedata->Pnum]->CurPiece[i]);
            dest.x=Gamedata->boards[Gamedata->Pnum]->CurPiece[i].x * Gamedata->images->SingleP->w;
            dest.y=Gamedata->boards[Gamedata->Pnum]->CurPiece[i].y * Gamedata->images->SingleP->h;
            SDL_BlitSurface(Gamedata->images->SingleP,NULL,gui->GetButtonSurface(Gamedata->Pnum * 100 + 100),&dest);
        }
        // Render the next piece
        for (int i=0; i < 2;++i) {
            RenderPiece(&Gamedata->boards[Gamedata->Pnum]->NxtPiece[i]);
            SDL_Surface *temp=SDL_ConvertSurface(Gamedata->images->SingleP, screen->format, 0);
            gui->ChangeButtonSurface(10+i,temp);
	   }
    }
    // Render the specials
    int xoff=int(double(Gamedata->settings->X)/ 640);
    int yoff=int(double(Gamedata->settings->Y)/ 480);
    src.x=gui->GetBXcord(15)*xoff;src.y=gui->GetBYcord(15)*yoff;src.w=Gamedata->images->Specials->w;src.h=Gamedata->images->Specials->h;
    SDL_BlitSurface(gui->GetBG(),&src,gui->GetButtonSurface(15),NULL);
    src.x=0; src.y=0; src.w=Gamedata->images->Specials->w/7;src.h=Gamedata->images->Specials->h;
    dest.x=0;dest.y=0;dest.w=src.w;dest.h=src.h;
    for (int i=0;i < 10;i++)
        if (Gamedata->boards[Gamedata->Pnum]->specials[i] != 0) {
            dest.x=i*dest.w; src.x=(Gamedata->boards[Gamedata->Pnum]->specials[i]-1) * src.w;
            SDL_BlitSurface(Gamedata->images->Specials,&src,gui->GetButtonSurface(15),&dest);
        }
    gui->ReDrawButton(15);
	return;
}

void RenderPiece(Piece* piece)
{
    // Copy original palette into the temp Piece surface
    SDL_SetPalette(Gamedata->images->SingleP, SDL_LOGPAL, Gamedata->images->LargePs->format->palette->colors, 0, 256);
    
    // The transparent color is stored in the 127th position in the palette    
    SDL_Color *trans=&Gamedata->images->SingleP->format->palette->colors[127];
    SDL_SetColorKey(Gamedata->images->SingleP, SDL_SRCCOLORKEY, SDL_MapRGB(Gamedata->images->SingleP->format,trans->r,trans->g,trans->b));
	
    SDL_Rect src;
    SDL_Color colors[128];
    
    if (piece->color == 0) {
        RenderLClear(piece->frame);
        return;
    }
    
    for (int i=0; i < 128;++i) {
        colors[i].r=Gamedata->images->LargePs->format->palette->colors[i+128].r;
        colors[i].g=Gamedata->images->LargePs->format->palette->colors[i+128].g;
        colors[i].b=Gamedata->images->LargePs->format->palette->colors[i+128].b;
    }
	int state=0;
	if (piece->state[0])
		state += 0x1;
	if (piece->state[1])
		state += 0x1 << 1;
	if (piece->state[2])
		state += 0x1 << 2;
	if (piece->state[3])
		state += 0x1 << 3;
 
	src.x=((state % 4) * Gamedata->images->SingleP->h);
	src.y=(int(state / 4) * Gamedata->images->SingleP->h); 
    src.y=src.y + (piece->frame * (Gamedata->images->SingleP->h * 4));
	src.w=Gamedata->images->SingleP->w;
	src.h=Gamedata->images->SingleP->h;
	
	// We only modify colors 128 and up
	switch (piece->color) {
        case 1:
        for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->LargePs->format->palette->colors[i+128].r;
			colors[i].g=0;
			colors[i].b=0;
		}
		break;
        case 2:
        for (int i=0;i < 128;++i) {
			colors[i].r=0;
			colors[i].g=Gamedata->images->LargePs->format->palette->colors[i+128].g;
			colors[i].b=0;
		}
		break;
        case 3:
		for (int i=0;i < 128;++i) {
			colors[i].r=0;
			colors[i].g=0;
			colors[i].b=Gamedata->images->LargePs->format->palette->colors[i+128].b;
		}
		break;
        case 4:
		for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->LargePs->format->palette->colors[i+128].r;;
			colors[i].g=0;
			colors[i].b=Gamedata->images->LargePs->format->palette->colors[i+128].b;
		}
		break;
        case 5:
		for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->LargePs->format->palette->colors[i+128].r;
			colors[i].g=Gamedata->images->LargePs->format->palette->colors[i+128].g;
			colors[i].b=0;
		}
		break;
        case 6:
		for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->LargePs->format->palette->colors[i+128].r;
			colors[i].g=Gamedata->images->LargePs->format->palette->colors[i+128].g;
			colors[i].b=Gamedata->images->LargePs->format->palette->colors[i+128].b;
		}
	}	
	SDL_BlitSurface(Gamedata->images->LargePs,&src,Gamedata->images->SingleP,NULL);
    SDL_SetPalette(Gamedata->images->SingleP, SDL_LOGPAL, colors, 128, 128);
	return;
}

void RenderSPiece(Piece* piece)
{
    // Copy original palette into the temp Piece surface
    SDL_SetPalette(Gamedata->images->SSingleP, SDL_LOGPAL, Gamedata->images->SmallPs->format->palette->colors, 0, 256);
    
    // The transparent color is stored in the 127th position in the palette    
    SDL_Color *trans=&Gamedata->images->SSingleP->format->palette->colors[127];
    SDL_SetColorKey(Gamedata->images->SSingleP, SDL_SRCCOLORKEY, SDL_MapRGB(Gamedata->images->SSingleP->format,trans->r,trans->g,trans->b));
	
    SDL_Rect src;
    SDL_Color colors[128];
    
    if (piece->color == 0) {
        RenderSClear(piece->frame);
        return;
    }
    
    for (int i=0; i < 128;++i) {
        colors[i].r=Gamedata->images->SmallPs->format->palette->colors[i+128].r;
        colors[i].g=Gamedata->images->SmallPs->format->palette->colors[i+128].g;
        colors[i].b=Gamedata->images->SmallPs->format->palette->colors[i+128].b;
    }
	int state=0;
	if (piece->state[0])
		state += 0x1;
	if (piece->state[1])
		state += 0x1 << 1;
	if (piece->state[2])
		state += 0x1 << 2;
	if (piece->state[3])
		state += 0x1 << 3;
 
	src.x=((state % 4) * Gamedata->images->SSingleP->h);
	src.y=(int(state / 4) * Gamedata->images->SSingleP->h); 
    src.y=src.y + (piece->frame * (Gamedata->images->SSingleP->h * 4));
	src.w=Gamedata->images->SSingleP->w;
	src.h=Gamedata->images->SSingleP->h;
	
	// We only modify colors 128 and up
	switch (piece->color) {
        case 1:
        for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->SmallPs->format->palette->colors[i+128].r;
			colors[i].g=0;
			colors[i].b=0;
		}
		break;
        case 2:
        for (int i=0;i < 128;++i) {
			colors[i].r=0;
			colors[i].g=Gamedata->images->SmallPs->format->palette->colors[i+128].g;
			colors[i].b=0;
		}
		break;
        case 3:
		for (int i=0;i < 128;++i) {
			colors[i].r=0;
			colors[i].g=0;
			colors[i].b=Gamedata->images->SmallPs->format->palette->colors[i+128].b;
		}
		break;
        case 4:
		for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->SmallPs->format->palette->colors[i+128].r;;
			colors[i].g=0;
			colors[i].b=Gamedata->images->SmallPs->format->palette->colors[i+128].b;
		}
		break;
        case 5:
		for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->SmallPs->format->palette->colors[i+128].r;
			colors[i].g=Gamedata->images->SmallPs->format->palette->colors[i+128].g;
			colors[i].b=0;
		}
		break;
        case 6:
		for (int i=0;i < 128;++i) {
			colors[i].r=Gamedata->images->SmallPs->format->palette->colors[i+128].r;
			colors[i].g=Gamedata->images->SmallPs->format->palette->colors[i+128].g;
			colors[i].b=Gamedata->images->SmallPs->format->palette->colors[i+128].b;
		}
	}	
	SDL_BlitSurface(Gamedata->images->SmallPs,&src,Gamedata->images->SSingleP,NULL);
    SDL_SetPalette(Gamedata->images->SSingleP, SDL_LOGPAL, colors, 128, 128);
    
	return;
}

void RenderLClear(int frame)
{
    SDL_Rect src;
    src.x=0; src.y=frame * Gamedata->images->SingleP->h; src.w=Gamedata->images->SingleP->w;
    src.h=Gamedata->images->SingleP->h;
    
    SDL_BlitSurface(Gamedata->images->LargeCs,&src,Gamedata->images->SingleP,NULL);
    return;
}

void RenderSClear(int frame)
{
    SDL_Rect src;
    src.x=0; src.y=frame * Gamedata->images->SSingleP->h; src.w=Gamedata->images->SSingleP->w;
    src.h=Gamedata->images->SSingleP->h;
    
    SDL_BlitSurface(Gamedata->images->SmallCs,&src,Gamedata->images->SSingleP,NULL);
    return;
}

void RenderHighScores(GUI* gui)
{
    char temp[512];
    int size=Gamedata->scores->List.size();
    if (size > 10)
        size=10;
    for (int i=0; i < size;++i) {
        sprintf(temp,"%d. %s - %d%c",i,Gamedata->scores->List[i]->name,Gamedata->scores->List[i]->score,'\0');
        gui->ChangeText(10+i,temp,1);
    }
    Gamedata->UpdateScores=false;
    gui->ReDraw();
    return;
}
