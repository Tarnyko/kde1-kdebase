/*
  This file holfd the definitions for all classes used to
  display a properties dialog.
  */

#ifndef KFMPROPS_H
#define KFMPROPS_H

#include <qtabdlg.h>
#include <qradiobt.h>
#include <qlabel.h>
#include <qlayout.h>
#include <stdio.h>
#include <qstring.h>
#include <qchkbox.h>
#include <qlined.h>
#include <qlist.h>
#include <qcombo.h>
#include <qgrpbox.h>
#include <qpushbt.h>
#include <qlistbox.h>

#include <kurl.h>
#include <qpixmap.h>

class Properties;

/// A Page in the Properties dialog
/**
  This is an abstract class. You must inherit from this class
  to build a new kind of page.
  */
class PropsPage : public QWidget
{
public:
    /// Constructor
    PropsPage( Properties *_props );

    /// Returns the name that should appear in the tab.
    virtual const char* getTabName() { return ""; }
    /// Apply all changes to the file.
    /**
      This function is called when the user presses 'Ok'. The last page inserted
      is called first.
      */
    virtual void applyChanges() { }
    
protected:
    /// Pointer to the dialog
    Properties *properties;
};

/// 'General' page
/**
  This page displays the name of the file, its size and access times.
  */
class FilePropsPage : public PropsPage
{
public:
    /// Constructor
    FilePropsPage( Properties *_props );

    virtual const char* getTabName() { return "General"; }
    /// Applies all changes made
    /** 'General' must be always the first page in the dialog, since this
      function may rename the file which may confuse other applyChanges
      functions. When this page is the first one this means that this applyChanges
      function is the last one called.
      */
    virtual void applyChanges();
    
    /// Tests wether the file specified by _kurl needs a 'General' page.
    static bool supports( KURL *_kurl );
    
protected:
    QLineEdit *name;
    QLineEdit *fname;
    QLineEdit *lname;

    /// The initial filename
    QString oldName;
};

/// 'Permissions' page
/**
  In this page you can modify permissions and change
  the owner of a file.
  */
class FilePermissionsPropsPage : public PropsPage
{
public:
    /// Constructor
    FilePermissionsPropsPage( Properties *_props );

    virtual const char* getTabName() { return "Permissions"; }
    virtual void applyChanges();

    /// Tests wether the file specified by _kurl needs a 'Permissions' page.
    static bool supports( KURL *_kurl );
    
protected:
    QCheckBox *permUR;
    QCheckBox *permUW;
    QCheckBox *permUX;
    QCheckBox *permGR;
    QCheckBox *permGW;
    QCheckBox *permGX;
    QCheckBox *permOR;
    QCheckBox *permOW;
    QCheckBox *permOX;

    QLineEdit *grp;
    QLineEdit *owner;

    /// Old permissions
    int permissions;
    /// Old group
    QString strGroup;
    /// Old owner
    QString strOwner;
};

/// The main class.
/**
  This one is visible to the one who created the dialog.
  It brings up a QTabDialog.
  */
class Properties : public QObject
{
    Q_OBJECT
public:
    /// Bring up a dialog for the given URL.
    Properties( const char *_url );
    ~Properties();

    /// Returns a parsed URL
    KURL* getKURL() { return kurl; }
    /// Returns the URL text
    const char* getURL() { return url.data(); }
    /// Returns a pointer to the dialog
    QTabDialog* getTab() { return tab; }

    /// Usually called from a PropsPage in order of emitting the signal.
    void emitPropertiesChanged( const char *_new_name );
    
public slots:
    /// Called when the user presses 'Ok'.
    void slotApply();

signals:
    /// Notify about changes in properties and rnameings.
   /**
     For example the root widget needs to be informed about renameing. Otherwise
     it would consider the renamed icon a new icon and would move it to the upper left
     corner or something like that.
     In most cases you wont need this signal because KIOManager is informed about changes.
     This causes KFileWindow for example to reload its contents if neccessary.
     If the name did not change, _name is 0L.
     */
    void propertiesChanged( const char *_url, const char *_new_name );
    
protected:
    /// Inserts all pages in the dialog.
    void insertPages();

    /// The URL of the file
    QString url;
    /// The parsed URL
    KURL *kurl;
    /// List of all pages inserted ( first one first )
    QList<PropsPage> pageList;
    /// The dialog
    QTabDialog *tab;
};

/// Edit "KDE Desktop Entry" Files.
/**
  Used to edit the files containing
  [KDE Desktop Entry]
  Type=Application

  Such files are used to represent a program in kpanel and kfm.
  */
class ExecPropsPage : public PropsPage
{
    Q_OBJECT
public:
    /// Constructor
    ExecPropsPage( Properties *_props );

    virtual const char* getTabName() { return "Execute"; }
    virtual void applyChanges();

    static bool supports( KURL *_kurl );

public slots:
    void slotIconChanged( int );
    void slotBrowseExec();
    
protected:
    void drawIcon();
    virtual void paintEvent ( QPaintEvent *);
    
    QLineEdit *execEdit;
    QLineEdit *pathEdit;
    QComboBox *iconBox;
    QCheckBox *terminalCheck;
    QLineEdit *terminalEdit;
    QButton *execBrowse;

    QString execStr;
    QString pathStr;
    QString iconStr;
    QString termStr;
    QString termOptionsStr;

    QPixmap pixmap;
    QString pixmapFile;
};

/// Edit "KDE Desktop Entry" Files.
/**
  Used to edit the files containing
  [KDE Desktop Entry]
  URL=....

  Such files are used to represent a program in kpanel and kfm.
  */
class URLPropsPage : public PropsPage
{
    Q_OBJECT
public:
    /// Constructor
    URLPropsPage( Properties *_props );

    virtual const char* getTabName() { return "URL"; }
    virtual void applyChanges();

    static bool supports( KURL *_kurl );

public slots:
    void slotIconChanged( int );
    
protected:
    void drawIcon();
    virtual void paintEvent ( QPaintEvent *);
    
    QLineEdit *URLEdit;
    QComboBox *iconBox;

    QString URLStr;
    QString iconStr;

    QPixmap pixmap;
    QString pixmapFile;
};

class DirPropsPage : public PropsPage
{
    Q_OBJECT
public:
    /// Constructor
    DirPropsPage( Properties *_props );

    virtual const char* getTabName() { return "Dir"; }
    virtual void applyChanges();

    static bool supports( KURL *_kurl );

public slots:
    void slotIconChanged( int );
    void slotWallPaperChanged( int );
    void slotApply();
    void slotApplyGlobal();
    
protected:
    void drawIcon();
    void drawWallPaper();
    virtual void paintEvent ( QPaintEvent *);
    
    QPushButton *applyButton;
    QPushButton *globalButton;
    
    QComboBox *iconBox;
    QComboBox *wallBox;

    QString wallStr;
    QString iconStr;

    QPixmap pixmap;
    QString pixmapFile;

    QPixmap wallPixmap;
    QString wallFile;

};

/// Edit "KDE Desktop Entry" Files.
/**
  Used to edit the files containing
  [KDE Desktop Entry]
  Type=Application

  Such files are used to represent a program in kpanel and kfm.
  */
class ApplicationPropsPage : public PropsPage
{
    Q_OBJECT
public:
    /// Constructor
    ApplicationPropsPage( Properties *_props );

    virtual const char* getTabName() { return "Application"; }
    virtual void applyChanges();

    static bool supports( KURL *_kurl );

public slots:
    void slotDelExtension();
    void slotAddExtension();    

protected:
    
    QLineEdit *binaryPatternEdit;
    QLineEdit *commentEdit;
    QCheckBox *protocolFTP;
    QCheckBox *protocolHTTP;
    QCheckBox *protocolFILE;
    QCheckBox *protocolTAR;
    QCheckBox *protocolINFO;
    QCheckBox *protocolMAN;
    QListBox  *extensionsList;
    QListBox  *availableExtensionsList;
    QPushButton *addExtensionButton;
    QPushButton *delExtensionButton;

    QString protocolsStr;
    QString extensionsStr;
    QString binaryPatternStr;
    QString commentStr;
};

/// Edit "KDE Desktop Entry" Files.
/**
  Used to edit the files containing
  [KDE Desktop Entry]
  Type=FileType

  Such files are used to represent a program in kpanel and kfm.
  */
class BindingPropsPage : public PropsPage
{
    Q_OBJECT
public:
    /// Constructor
    BindingPropsPage( Properties *_props );

    virtual const char* getTabName() { return "Binding"; }
    virtual void applyChanges();

    static bool supports( KURL *_kurl );

public slots:
    void slotIconChanged( int );
    
protected:
    void drawIcon();
    virtual void paintEvent ( QPaintEvent *);
    
    QLineEdit *commentEdit;
    QLineEdit *patternEdit;
    QComboBox *iconBox;
    QComboBox *appBox;

    QString commentStr;
    QString patternStr;
    QString iconStr;
    QString appStr;

    QPixmap pixmap;
    QString pixmapFile;
};

#endif
