#ifndef kfm_h
#define kfm_h

#include <qwidget.h>
#include <qtimer.h>
#include <qstrlist.h>

#include <kiconloader.h>

#define pkfm KFM::kfm()

class KFM : public QWidget
{
    Q_OBJECT
public:
    KFM();
    ~KFM();
    
    KIconLoader *iconLoader() { return pIconLoader; }
    static KFM* kfm() { return pKfm; }
    static QStrList* history() { return pHistory; }
    static void addToHistory( const char *_url );
    /**
     * @return false on error.
     */
    static bool saveHTMLHistory( const char *_filename );
  
public slots:
    void slotSave();
    void slotTouch();
  
protected:
    KIconLoader *pIconLoader;
    QTimer timer;

    static KFM *pKfm;
    static QStrList *pHistory;
};

#endif
