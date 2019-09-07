//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id: G4UIQt.hh,v 1.16 2009/02/16 11:40:26 lgarnier Exp $
// GEANT4 tag $Name: geant4-09-03-patch-01 $
//
#ifndef G4UIQt_h
#define G4UIQt_h 

#if defined(G4UI_BUILD_QT_SESSION) || defined(G4UI_USE_QT)

#include <map>

#include "G4VBasicShell.hh"
#include "G4VInteractiveSession.hh"

#include <qobject.h>
#include <qmap.h>

class QMainWindow;
class QLineEdit;
class G4UIsession;
#if QT_VERSION < 0x040000
class QListView;
class QListViewItem;
#else
class QListWidget;
class QTreeWidget;
class QTreeWidgetItem;
#endif
class QTextEdit;
class QLabel;
class QDialog;

// Class description :
//
//  G4UIQt : class to handle a Qt interactive session.
// G4UIQt is the Qt version of G4UIterminal.
//
//  A command box is at disposal for entering/recalling Geant4 commands.
//  A menubar could be customized through the AddMenu, AddButton methods.
//  Note that there are corresponding Geant4 commands to add a 
// menus in the menubar and add buttons in a menu.
//  Ex : 
//    /gui/addMenu   test Test
//    /gui/addButton test Init /run/initialize
//    /gui/addButton test "Set gun" "/control/execute gun.g4m"
//    /gui/addButton test "Run one event" "/run/beamOn 1"
//
//  Command completion, by typing "tab" key, is available on the 
// command line.
//
// Class description - end :

class G4UIQt : public QObject, public G4VBasicShell, public G4VInteractiveSession {
  Q_OBJECT

public: // With description
  G4UIQt(int,char**);
  // (argv, argc) or (0, NULL) had to be given.
  G4UIsession* SessionStart();
  // To enter interactive X loop ; waiting/executing command,...
  void AddMenu(const char*,const char*);
  // To add a pulldown menu in the menu bar. 
  // First argument is the name of the menu.
  // Second argument is the label of the cascade button.
  // Ex : AddMenu("my_menu","My menu")
  void AddButton(const char*,const char*,const char*);
  // To add a push button in a pulldown menu.
  // First argument is the name of the menu.
  // Second argument is the label of the button.
  // Third argument is the Geant4 command executed when the button is fired.
  // Ex : AddButton("my_menu","Run","/run/beamOn 1"); 
public:
  ~G4UIQt();
  void Prompt(G4String);
  void SessionTerminate();
  void PauseSessionStart(G4String);
  G4int ReceiveG4cout(G4String);
  G4int ReceiveG4cerr(G4String);
  //   G4String GetCommand(Widget);
  QMainWindow * getMainWindow();

private:
  void SecondaryLoop(G4String); // a VIRER
  void TerminalHelp(G4String);
#if QT_VERSION < 0x040000
  QListView * CreateHelpTree();
#else
  QTreeWidget * CreateHelpTree();
#endif

#if QT_VERSION < 0x040000
  void CreateChildTree(QListViewItem*,G4UIcommandTree*);
  QListViewItem* FindTreeItem(QListViewItem *,const QString&);
#else
  void CreateChildTree(QTreeWidgetItem*,G4UIcommandTree*);
  QTreeWidgetItem* FindTreeItem(QTreeWidgetItem *,const QString&);
#endif

  QString GetCommandList(const G4UIcommand*);

  G4bool GetHelpChoice(G4int&) ;// have to be implemeted because we heritate from G4VBasicShell
  void ExitHelp();// have to be implemeted because we heritate from G4VBasicShell
  bool eventFilter(QObject*,QEvent*);
  void ActivateCommand(G4String);
  QMap<int,QString> LookForHelpStringInChildTree(G4UIcommandTree *,const QString&);


private:

  //  Widget form,shell,command,menuBar,text;
  //  std::map<QAction*,G4String, std::less<QAction*> > commands;
  //  static void commandEnteredCallback(Widget,XtPointer,XtPointer);
  //  static void keyHandler(Widget,XtPointer,XEvent*,Boolean*);
  QMainWindow * fMainWindow;
  QLabel *fCommandLabel;
  QLineEdit * fCommandArea;
  QTextEdit *fTextArea;
  QTextEdit *fHelpArea;
#if QT_VERSION < 0x040000
  QListView *fCommandHistoryArea;
  QListView *fHelpTreeWidget;
#else
  QListWidget *fCommandHistoryArea;
  QTreeWidget *fHelpTreeWidget;
#endif
  QDialog *fHelpDialog;
  QLineEdit *helpLine;
 
signals : 
  void myClicked(const QString &text);

private slots :
  void ExitSession();
  void ClearButtonCallback();
  void CommandEnteredCallback();
  void ButtonCallback(const QString&);
  void HelpTreeClicCallback();
  void HelpTreeDoubleClicCallback();
  void ShowHelpCallback();
  void CommandHistoryCallback();
  void lookForHelpStringCallback();
};

#endif

#endif

