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
#include <st/linuxserial.h>
#include <st/sexp.h>
#include <st/target.h>

#include <cassert>
#include <iostream>
#include <sstream>
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
  void SaveFile(const char *path, BTextView *text);
  void FrameResized(float w, float h);

private:
  void UpdateTextRect(void);
  void EvaluateAndPrint(const char *buf, std::size_t size);

  BMenuBar *fMenuBar;
  BTextView *fTextView, *fConsoleView;
  BFilePanel *fOpenPanel, *fSavePanel;
  BString fFilePath;
  char fEvalBuf[MAX_TEXT_LENGTH]{};
  LispTarget *fTarget{};
  LinuxSerialPort *fSerial{};
  bool savingOutput{};
};

enum STEvents {
  M_FILE_NEW = 'flnw',
  M_SHOW_OPEN = 'shop',
  M_SAVE = 'save',
  M_SAVE_AS = 'svas',
  M_SAVE_OUTPUT = 'svot',
  M_CONNECT = 'conn',
  M_RUN_SEL = 'runs',
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
    : BWindow(BRect(100, 100, 600, 500), "SuperTyson", B_TITLED_WINDOW,
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

  menu->AddSeparatorItem();
  menu->AddItem(new BMenuItem("Save output", new BMessage(M_SAVE_OUTPUT), 'P'));

  // Add the text views and their scrollbars.
  fTextView = createTextView(true);
  fConsoleView = createTextView(false);

  BSplitView *splitview
      = BLayoutBuilder::Split<>(B_HORIZONTAL)
            .Add(new BScrollView("scrollview", fTextView, B_FOLLOW_ALL, 0, false, true), 1.0f)
            .Add(new BScrollView("scrollview", fConsoleView, B_FOLLOW_ALL, 0, false, true), 1.0f);

  // TODO: add a proper icon
  BButton *runButton = new BButton("run", "Run file", new BMessage(M_RUN));
  BButton *runSelButton = new BButton("run_sel", "Run selection", new BMessage(M_RUN_SEL));
  BButton *connectButton = new BButton("connect", "Connect serial port", new BMessage(M_CONNECT));
  BGroupLayout *buttons
      = BLayoutBuilder::Group<>(B_HORIZONTAL).Add(runButton).Add(runSelButton).Add(connectButton);

  // Build the final layout for the app window
  BLayoutBuilder::Group<>(this, B_VERTICAL)
      .Add(fMenuBar)
      .Add(buttons)
      .Add(splitview)
      .SetInsets(0, 0, 0, 0)  // necessary for menu bar to stick to top of window
      .End();

  // Focus the text view on application start.
  fTextView->MakeFocus(true);

  // Adjust window size to fit content
  ResizeToPreferred();

  // Calculate minimum size
  BSize minSize = GetLayout()->MinSize();

  // Set minimum size constraints
  SetSizeLimits(minSize.width, B_SIZE_UNSET, minSize.height, B_SIZE_UNSET);

  // *End of visible components / window layout*

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
}

MainWindow::~MainWindow(void) {
  // TODO: could we just use RAII for those?
  delete fOpenPanel;
  delete fSavePanel;
}

// TODO: connect stub to actual evaluator
auto evaluate(std::string_view &contents, LispTarget *target) -> std::string {
  // Welp, that's ugly AF
  std::string copy(contents);

  if (target == nullptr) {
    return "not connected to board.";
  }

  // FIXME: we should be able to evaluate not only s-exps, ie printing literals
  // or variable values.
  // std::stringstream is(copy);
  // Sexp exp;
  // is >> exp;

  return target->evaluate(copy);
}

void MainWindow::EvaluateAndPrint(const char *buf, std::size_t size) {
  // TODO: make visible to user max text size (maybe ulisp also has limits)
  // TODO: block until previous evaluation is completed
  fEvalBuf[MAX_TEXT_LENGTH - 1] = 0;

  std::string_view view(fEvalBuf);

  std::string result = evaluate(view, fTarget);
  std::cout << result << std::endl;

  std::string separator{};
  separator += '\n';
  separator += "--------------------------";
  separator += '\n';
  // TODO: better name for this var
  // TODO: add option to enable/disable echo
  separator += fEvalBuf;
  separator += '\n';
  separator += "=> ";

  result.insert(0, separator);
  fConsoleView->Insert(result.c_str());
  fConsoleView->ScrollToOffset(fConsoleView->TextLength());
}

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
        SaveFile(fFilePath.String(), fTextView);
      }
      break;
    }

    case M_SAVE_AS: {
      fSavePanel->Show();
      break;
    }

    case M_SAVE_OUTPUT: {
      // ewww
      savingOutput = true;
      fSavePanel->Show();
      break;
    }

    case B_SAVE_REQUESTED: {
      entry_ref dir;
      BString name;
      if (msg->FindRef("directory", &dir) == B_OK && msg->FindString("name", &name) == B_OK) {
        BPath path(&dir);
        path.Append(name);
        SaveFile(path.Path(), savingOutput ? fConsoleView : fTextView);
      }
      savingOutput = false;
      break;
    }

    case M_CONNECT: {
      if (fTarget != nullptr) {
        // We are already connected. Destroy the current target connection
        // before establishing a new one.
        delete fTarget;
        delete fSerial;
      }

      // FIXME: allow user port/baud/fc selection
      std::string uart_path = "/dev/ports/usb0";
      fSerial = new LinuxSerialPort(uart_path.c_str(), 115200, false);

      // maybe change the API to allow a pointer too
      LinuxSerialPort &serial = *fSerial;
      fTarget = new LispTarget(serial);

      break;
    }

    case M_RUN: {
      fTextView->GetText(0, sizeof(fEvalBuf), fEvalBuf);
      EvaluateAndPrint(fEvalBuf, sizeof(fEvalBuf));
      break;
    }

    case M_RUN_SEL: {
      int32 start, end, len;
      fTextView->GetSelection(&start, &end);
      len = end - start;

      assert(len <= sizeof(fEvalBuf));
      fTextView->GetText(start, len, fEvalBuf);
      EvaluateAndPrint(fEvalBuf, sizeof(fEvalBuf));
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

void MainWindow::SaveFile(const char *path, BTextView *text) {
  // Take a string path and save the BTextView data to the file. File is either
  // created or overwritten.
  BFile file;
  if (file.SetTo(path, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE) != B_OK) return;

  if (BTranslationUtils::PutStyledText(text, &file) == B_OK) {
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
