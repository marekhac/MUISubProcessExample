/* compile with "gcc -noixemul -o subprocess_example subprocess_example.c" */

#include <dos/dostags.h>
#include <proto/muimaster.h>
#include <proto/intuition.h>
#include "api_MUI.h"

Object *app, *Win, *StartButton, *StopButton, *TextField;

struct Library *MUIMasterBase;
struct Hook Click_button_hook;

void childprocesscode(void); 

struct Process *childprocess = NULL;
UBYTE childprocessname[] = "childprocess";

int counter = 0;
BOOL stopSubProc = FALSE;

long BuildApplication (void)
{
	app = ApplicationObject,
		MUIA_Application_Author, (ULONG)"Marek Hac",
		MUIA_Application_Base, (ULONG)"MUI Test",
		MUIA_Application_Copyright, (ULONG)"MarX",
		MUIA_Application_Title, (ULONG)"MUI",
		SubWindow,
			Win = WindowObject,
				MUIA_Window_Title, (ULONG)"MUI Test",	
				WindowContents,
				VGroup,
				
					// GROUP : HEADER
				
					Child, HGroup,
                    	GroupFrame,
                    	MUIA_Background, MUII_GroupBack,
                   	 	MUIA_Group_Rows, 1,
                    	MUIA_Group_Columns, 2,
    
						Child, HGroup, 
				        	Child, TextObject,
				          		MUIA_Text_Contents, (long)"\33rSubProcess Said:",
				          	  	MUIA_Text_SetMax, TRUE,
				         	End,
				        	
				        	Child, TextField = TextObject, 
        						MUIA_Frame, MUIV_Frame_Text,
        						MUIA_Background, MUII_TextBack,
       						End,
				      	End,	
					End, // end header info group
					
					// GROUP : BUTTONS
					
					Child, HGroup, 
						GroupFrame,
		        		MUIA_Background, MUII_GroupBack,
		        		MUIA_Group_Rows, 1,
		 	    		MUIA_Group_Columns, 3,	
				
			 	   		Child, StartButton = TextObject,
			         		MUIA_Frame,  MUIV_Frame_Button,
			         		MUIA_Background, MUII_ButtonBack,
			         		MUIA_Font, MUIV_Font_Button,
			         		MUIA_InputMode,  MUIV_InputMode_RelVerify,
			         		MUIA_Text_Contents,  (long)"\33cRun SubProcess",
			        	End,
			        	
			        	Child, StopButton = TextObject,
			         		MUIA_Frame,  MUIV_Frame_Button,
			         		MUIA_Background, MUII_ButtonBack,
			         		MUIA_Font, MUIV_Font_Button,
			         		MUIA_InputMode,  MUIV_InputMode_RelVerify,
			         		MUIA_Text_Contents,  (long)"\33cStop SubProcess",
			        	End,
			        	
					End,
				End, // end VGroup
            End,
        End;
}

// sub process methods

void startNewProc()
{
    if (childprocess = (struct Process *) CreateNewProcTags(
                        NP_Entry,       childprocesscode,  
                        NP_Name,        childprocessname,
                        NP_Priority, 	1,
                        NP_CodeType, 	CODETYPE_PPC,
                       TAG_DONE))

    {	
    	printf("Subprocess created\n");    	
    }
    else 
    {
        printf("Can't create sub process\n");
	}
}

void childprocesscode(void)  
{ 	
	while (!stopSubProc) 
	{
    	Delay(100);
    	
    	DoMethod (app, MUIM_Application_PushMethod, TextField, 4, MUIM_SetAsString, MUIA_Text_Contents, "%ld", counter);

    	printf("SubProcess Said: %d\n", counter);
    	counter++;	
    }   
}

M_HOOK(Start_button , APTR obj, APTR dana)
{
	stopSubProc = FALSE;
	startNewProc();
}

M_HOOK(Stop_button , APTR obj, APTR dana)
{
	stopSubProc = TRUE;
	childprocess = NULL;
	printf("SubProcess stopped\n");
}

void SetNotifications (void)
{
	DoMethod(Win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, app, 2,
	 		  MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    
    DoMethod (StartButton, MUIM_Notify, MUIA_Pressed, FALSE, app,
              2, MUIM_CallHook, &Start_button_hook);
              
    DoMethod (StopButton, MUIM_Notify, MUIA_Pressed, FALSE, app,
              2, MUIM_CallHook, &Stop_button_hook);
}


void MainLoop (void)
{
	ULONG signals = 0;

	set(Win, MUIA_Window_Open, TRUE);
			 
	while (DoMethod(app, MUIM_Application_NewInput, &signals) != MUIV_Application_ReturnID_Quit)
	{
		signals = Wait(signals | SIGBREAKF_CTRL_C);
		if (signals & SIGBREAKF_CTRL_C) break;
	}
	
	stopSubProc = TRUE;

	set(Win, MUIA_Window_Open, FALSE);

}

int main (int argc, char **argv)
{

  if (IntuitionBase = (struct IntuitionBase*)OpenLibrary ("intuition.library", 37))
   {
    if (MUIMasterBase = (struct MUIMasterBase*) OpenLibrary ("muimaster.library", 19))
     {
      if (BuildApplication ())
       {
        SetNotifications ();
        MainLoop ();
        MUI_DisposeObject (app);
       }
      CloseLibrary (MUIMasterBase);
     }
    CloseLibrary ((struct Library*)IntuitionBase);
   }
  return 0;   
}

