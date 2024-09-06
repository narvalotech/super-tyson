#include <Alert.h>
#include <Application.h>
#include <Button.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FilePanel.h>
#include <Layout.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Messenger.h>
#include <NodeInfo.h>
#include <Path.h>
#include <ScrollView.h>
#include <SplitView.h>
#include <String.h>
#include <TextView.h>
#include <TranslationUtils.h>
#include <Window.h>

#include <iostream>
#include <string>
#include <string_view>

const uint32_t MAX_TEXT_LENGTH{8192};

class MainWindow : public BWindow {
public:
  MainWindow(void);
  ~MainWindow(void);
  void MessageReceived(BMessage *msg);
  bool QuitRequested(void);

  void OpenFile(const entry_ref &ref);
  void SaveFile(const char *path);
  void FrameResized(float w, float h);

private:
  void UpdateTextRect(void);

  BMenuBar *fMenuBar;
  BTextView *fTextView, *fConsoleView;
  BFilePanel *fOpenPanel, *fSavePanel;
  BString fFilePath;
  char fEvalBuf[MAX_TEXT_LENGTH]{};
};

enum STEvents {
  M_FILE_NEW = 'flnw',
  M_SHOW_OPEN = 'shop',
  M_SAVE = 'save',
  M_SAVE_AS = 'svas',
  M_PRINT_SETUP = 'ptcf',
  M_PRINT = 'prin',
  M_RUN = 'runb'
};

BTextView *createTextView(bool editable) {
  BRect frame(0, 0, 200, 100);
  auto textview = new BTextView(frame, "textview", frame.OffsetToCopy(0, 0), B_FOLLOW_ALL_SIDES,
                                B_WILL_DRAW | B_FRAME_EVENTS);

  textview->SetStylable(false);
  textview->MakeEditable(editable);
  textview->SetAutoindent(true);

  return textview;
}

MainWindow::MainWindow(void)
    : BWindow(BRect(100, 100, 500, 500), "SuperTyson", B_TITLED_WINDOW,
              B_AUTO_UPDATE_SIZE_LIMITS | B_ASYNCHRONOUS_CONTROLS) {
  // Create and populate menu bar
  BRect r(Bounds());
  r.bottom = 20;

  fMenuBar = new BMenuBar(r, "menubar");

  BMenu *menu = new BMenu("File");
  fMenuBar->AddItem(menu);

  // Use shorthand way to add items to menu.
  // Use the standard haiku hotkeys (Alt+[key]).
  menu->AddItem(new BMenuItem("New", new BMessage(M_FILE_NEW), 'N'));
  menu->AddItem(new BMenuItem("Open", new BMessage(M_SHOW_OPEN), 'O'));
  menu->AddSeparatorItem();
  menu->AddItem(new BMenuItem("Save", new BMessage(M_SAVE), 'S'));

  // This one uses Alt + Shift
  menu->AddItem(new BMenuItem("Save As" B_UTF8_ELLIPSIS, new BMessage(M_SAVE_AS), 'S',
                              B_COMMAND_KEY | B_SHIFT_KEY));

  // Add the text views and their scrollbars.
  fTextView = createTextView(true);
  fConsoleView = createTextView(false);

  BSplitView *splitview
      = BLayoutBuilder::Split<>(B_HORIZONTAL)
            .Add(new BScrollView("scrollview", fTextView, B_FOLLOW_ALL, 0, false, true), 1.0f)
            .Add(new BScrollView("scrollview", fConsoleView, B_FOLLOW_ALL, 0, false, true), 1.0f);

  // FIXME: add a proper icon
  BButton *runButton = new BButton("run", "Run file", new BMessage(M_RUN));

  // Build the final layout for the app window
  BLayoutBuilder::Group<>(this, B_VERTICAL)
      .Add(fMenuBar)
      .Add(runButton)
      .Add(splitview)
      .SetInsets(0, 0, 0, 0)  // necessary for menu bar to stick to top of window
      .End();

  // BFilePanel shows the file pickers. Once constructed, call the Show() method
  // to enable the use to choose a file.
  //
  // Here we:
  // - attach the target to our window using a custom messenger
  // - start at the default directory (NULL)
  // - allow selecting files (B_FILE_NODE)
  // - do not allow multiple-selection (false)
  //
  // By default the "open" panel will send a B_REFS_RECEIVED message and the
  // "save" panel a B_SAVE_REQUESTED message.
  //
  // TODO: add a filter for *.lisp files
  BMessenger msgr(NULL, this);
  fOpenPanel = new BFilePanel(B_OPEN_PANEL, &msgr, NULL, B_FILE_NODE, false);
  fSavePanel = new BFilePanel(B_SAVE_PANEL, &msgr, NULL, B_FILE_NODE, false);

  // Focus the text view on application start.
  fTextView->MakeFocus(true);

  // Calculate minimum size
  BSize minSize = GetLayout()->MinSize();

  // Set minimum size constraints
  SetSizeLimits(minSize.width, B_SIZE_UNSET, minSize.height, B_SIZE_UNSET);

  // Adjust window size to fit content
  ResizeToPreferred();
}

MainWindow::~MainWindow(void) {
  // TODO: could we just use RAII for those two?
  delete fOpenPanel;
  delete fSavePanel;
}

// TODO: connect stub to actual evaluator
auto evaluate(std::string_view &contents) -> std::string { return std::string(contents); }

void MainWindow::MessageReceived(BMessage *msg) {
  switch (msg->what) {
    case M_FILE_NEW: {
      // Erase all the text. Reset file path to empty.
      // TODO: save dialog? or create a new tab?
      fTextView->SetText("");
      fFilePath = "";
      break;
    }

    // Open and save files
    case M_SHOW_OPEN: {
      fOpenPanel->Show();
      break;
    }

    case B_REFS_RECEIVED: {
      entry_ref ref;
      if (msg->FindRef("refs", &ref) != B_OK) break;
      OpenFile(ref);
      break;
    }

    case M_SAVE: {
      if (fFilePath.CountChars() < 1) {
        fSavePanel->Show();
      } else {
        SaveFile(fFilePath.String());
      }
      break;
    }

    case M_SAVE_AS: {
      fSavePanel->Show();
      break;
    }

    case B_SAVE_REQUESTED: {
      entry_ref dir;
      BString name;
      if (msg->FindRef("directory", &dir) == B_OK && msg->FindString("name", &name) == B_OK) {
        BPath path(&dir);
        path.Append(name);
        SaveFile(path.Path());
      }
      break;
    }

    case M_RUN: {
      // TODO: make a "run selection" button
      // TODO: make visible to user max text size (maybe ulisp also has limits)
      // TODO: block until previous evaluation is completed
      fTextView->GetText(0, sizeof(fEvalBuf), fEvalBuf);
      fEvalBuf[MAX_TEXT_LENGTH - 1] = 0;

      std::string_view view(fEvalBuf);

      std::string result = evaluate(view);
      std::cout << evaluate(view) << std::endl;

      std::string separator{};
      separator += '\n';
      separator += "--------------------------";
      separator += '\n';

      result.insert(0, separator);
      fConsoleView->Insert(result.c_str());
      fConsoleView->ScrollToOffset(fConsoleView->TextLength());

      break;
    }

    default:
      // Pass unhandled message to the parent BWindow class.
      // This is so it can react, to e.g. window close messages.
      BWindow::MessageReceived(msg);
      break;
  }
}

bool MainWindow::QuitRequested(void) {
  // Hook to add a "save before quit" message.
  be_app->PostMessage(B_QUIT_REQUESTED);
  return true;
}

void MainWindow::OpenFile(const entry_ref &ref) {
  // Convert symlinks to their targets.
  BEntry entry(&ref, true);
  entry_ref realRef;
  entry.GetRef(&realRef);

  // Use Translation Kit to read out file into editor.
  BFile file(&realRef, B_READ_ONLY);
  if (file.InitCheck() != B_OK) return;

  // Clear the text view before reading the new file.
  fTextView->SetText("");

  // Unfortunately, BTranslationUtils has no API documentation. It doesn't seem
  // like there is a method to get and put plain text, so we have to use the
  // "styled" one.
  if (BTranslationUtils::GetStyledText(&file, fTextView) == B_OK) {
    // BPath bridges the data classes from Storage Kit and string-based paths.
    // Set the BPath instance to the opened file's path and set the window title
    // to the filename.
    BPath path(&realRef);
    fFilePath = path.Path();
    SetTitle(path.Leaf());
  }
}

void MainWindow::SaveFile(const char *path) {
  // Take a string path and save the BTextView data to the file. File is either
  // created or overwritten.
  BFile file;
  if (file.SetTo(path, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE) != B_OK) return;

  if (BTranslationUtils::PutStyledText(fTextView, &file) == B_OK) {
    fFilePath = path;
    BNodeInfo nodeInfo(&file);
    nodeInfo.SetType("text/plain");
  }
}

void MainWindow::FrameResized(float w, float h) {
  // When a BTextView is resized, it does not automatically update its text
  // rectangle. That's why we overload this function to do it manually.
  UpdateTextRect();
}

void MainWindow::UpdateTextRect(void) {
  BRect r(fTextView->Bounds());
  r.InsetBy(5, 5);
  fTextView->SetTextRect(r);
}

class SuperTyson : public BApplication {
public:
  SuperTyson(void) : BApplication("application/x-vnd.dw-SuperTyson") {
    MainWindow *mainwin = new MainWindow();
    mainwin->Show();
  }
};

int main() {
  SuperTyson app;
  app.Run();

  return 0;
}
