/*		Listener.h - the Listener class - MAXScript listener windows
 *
 *		Copyright (c) John Wainwright, 1996
 *		
 */

#ifndef _H_LISTENER
#define _H_LISTENER

#include <windowsx.h>
#include "Pipe.h"
#include "Thunks.h"

// helper methods for editor windows
ScripterExport int get_cur_line(HWND window, TSTR& line, bool stripEOL);
ScripterExport void set_edit_box_font(HWND window, TCHAR* font_name);
ScripterExport void set_edit_box_font_size(HWND window, int fontSize);

extern GlobalThunk* listener_result_thunk;

extern ScripterExport BOOL end_keyboard_input;
extern ScripterExport BOOL start_keyboard_input;
extern ScripterExport MCHAR* keyboard_input;
extern ScripterExport Value* keyboard_terminator;
extern ScripterExport Array* keyboard_input_terminators;

// the Scintilla styles for the edit box
#define LISTENER_STYLE_INPUT	0
#define LISTENER_STYLE_OUTPUT	1
#define LISTENER_STYLE_MESSAGE	2
#define LISTENER_STYLE_LABEL    3

extern COLORREF input_text_color;
extern COLORREF output_text_color;
extern COLORREF message_text_color;
extern COLORREF label_text_color;

// listener flag values
#define LSNR_INPUT_MODE_MASK	0x000F
#define LSNR_KEYINPUT_OFF		0x0000
#define LSNR_KEYINPUT_LINE		0x0001
#define LSNR_KEYINPUT_CHAR		0x0002
#define LSNR_SHOWING			0x0010	// MAXScript is forcing a show, ignore all other ShowWindows
#define LSNR_NO_MACRO_REDRAW	0x0020	// disable drawing in macro-rec box (to get round bug in WM_SETREDRAW)

// listener flag values added for r10 to support scintilla control
#define LSNR_STYLE_MASK			0x0300	// listener style flag mask
#define LSNR_STYLE_INPUT		0x0000	// LISTENER_STYLE_INPUT set
#define LSNR_STYLE_OUTPUT		0x0100	// LISTENER_STYLE_OUTPUT set
#define LSNR_STYLE_MESSAGE		0x0200	// LISTENER_STYLE_MESSAGE set
#define LSNR_BLOCK_MINI_UPDATES	0x0400	// mini updates blocked

#define EDIT_BOX_ITEM			1001	// listener edit box dlg item #
#define MACROREC_BOX_ITEM		1002	// listener macro-recorder edit box dlg item #

class ListenerViewWindow;

class Listener : public Value
{
private:
	HANDLE		listener_thread;
	DWORD		thread_id;
	static bool	mBailout;				// for shutting down the listener thread. 
public:
	HWND		listener_window;		// main listener window
	HWND		edit_box;				// edit control for main type-in
	HWND		macrorec_box;			// edit control for macro-recorder output
	HWND		mini_listener;			// mini-listener parent window in the MAX status panel
	HWND		mini_edit;				// mini-listener edit control for type_in
	HWND		mini_macrorec;			// mini-listener edit control for macro-recorder output
	WindowStream* edit_stream;			// stream for the main edit box
	WindowStream* macrorec_stream;		// stream for the macro-recorder edit box
	WindowStream* mini_edit_stream;		// stream for the mini edit box
	WindowStream* mini_macrorec_stream;	// stream for the mini macro-recorder edit box
	Pipe*		source_pipe;			// the source pipe for the listener, source written to, compiler reads from
	int			flags;
	ListenerViewWindow* lvw;			// the ViewWindow instance for the listener

	//! \brief Constructor. This is statically allocated
	/*!	When this constructor gets called, it starts a new thread using the run method.*/
	Listener(HINSTANCE mxs_instance, HWND MAX_window);
	//! \brief Destructor.
	/*! Since the Listener is statically allocated, this destructor gets called long after
		Max has uninitialized itself, and deleted all Collectable objects. */
	~Listener();

	//! The method that gets run for the Listener thread
	static DWORD run(Listener *l);
	//! For shutting down the Listener thread
	void         stop();
	void		create_listener_window(HINSTANCE hInstance, HWND hwnd);

	void		gc_trace();
	void		collect() { delete this; }

	ScripterExport void set_keyinput_mode(int mode) { flags = (flags & ~LSNR_INPUT_MODE_MASK) | mode; }
	// get/set the current Scintilla style for the edit box
	ScripterExport int set_style(int style);
	ScripterExport int get_style();
	ScripterExport void set_font(TCHAR* font_name);
	ScripterExport void set_font_size(int fontSize);
	// get/set whether changes to listener windows update mini windows
	ScripterExport bool set_block_mini_updates(bool blocked);
	ScripterExport bool get_block_mini_updates();
};

// ViewWindow subclass for putting the listener in a MAX viewport
class ListenerViewWindow : public ViewWindow
{
public:
	MCHAR *GetName();
	HWND CreateViewWindow(HWND hParent, int x, int y, int w, int h);
	void DestroyViewWindow(HWND hWnd);
	BOOL CanCreate();
};

class ListenerMessageData 
{
public:
	WPARAM wParam;
	LPARAM lParam;
	HANDLE message_event;
	ListenerMessageData(WPARAM wp, LPARAM lp, HANDLE me) { wParam = wp; lParam = lp; message_event = me; }
};

#define CLICK_STACK_SIZE	8			// number of clicked-at locations to remember for ctrl-R return
struct edit_window
{
	edit_window*	next;
	edit_window*	prev;
	Value*			file_name;

	bool			dont_flag_save; // deprecated
	bool			needs_save;		// deprecated

	HWND			window;
	HWND			edit_box;
	int				sr_offset;
	int				click_tos;
	int			    click_at[CLICK_STACK_SIZE];
	bool			editing;
	IUnknown*		pDoc;

	INT_PTR			new_rollout();	
	INT_PTR			edit_rollout();	
	INT_PTR			new_rollout(bool delete_ew);	
	INT_PTR			edit_rollout(bool delete_ew);	
	static void		gc_trace();
	bool			GetNeedsSave();
	void			ClearNeedsSave();
};

#endif
